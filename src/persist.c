#include "persist.h"
#include "config.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/time.h"
#include <string.h>

#define FLASH_TARGET_OFFSET   (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

#define PERSIST_MAGIC         0x68656E64u  // "hend"

#define PERSIST_SETTLE_MS     2000u

typedef struct {
    uint32_t magic;
    uint8_t  mode[NUM_CHANNELS];
    uint8_t  op_mode[NUM_CHANNELS];
    uint32_t crc;
} persist_data_t;

static const persist_data_t *flash_data =
    (const persist_data_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

// Change-Tracking
static uint8_t  s_last_mode[NUM_CHANNELS]    = { 0xFF, 0xFF, 0xFF };
static uint8_t  s_last_op[NUM_CHANNELS]      = { 0xFF, 0xFF, 0xFF };
static uint64_t s_change_time_us             = 0u;
static bool     s_dirty                      = false;

static uint32_t calc_crc(const persist_data_t *d) {
    uint32_t crc = d->magic;
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        crc = crc * 31u + d->mode[i];
        crc = crc * 31u + d->op_mode[i];
    }
    return crc;
}

bool persist_load(hendl_t *h) {
    if (flash_data->magic != PERSIST_MAGIC) {
        return false;  // Never written
    }
    if (flash_data->crc != calc_crc(flash_data)) {
        return false;  // Corrupt
    }

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        uint8_t m  = flash_data->mode[i];
        uint8_t op = flash_data->op_mode[i];
        if (m  < NUM_MODES)            h->ch[i].mode    = (channel_mode_t)m;
        if (op <= OP_TRIGGERED)        h->ch[i].op_mode = (op_mode_t)op;

        s_last_mode[i] = (uint8_t)h->ch[i].mode;
        s_last_op[i]   = (uint8_t)h->ch[i].op_mode;
    }
    return true;
}

void persist_save(hendl_t *h) {
    persist_data_t data;
    memset(&data, 0, sizeof(data));
    data.magic = PERSIST_MAGIC;
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        data.mode[i]    = (uint8_t)h->ch[i].mode;
        data.op_mode[i] = (uint8_t)h->ch[i].op_mode;
    }
    data.crc = calc_crc(&data);

    uint8_t page[FLASH_PAGE_SIZE];
    memset(page, 0, sizeof(page));
    memcpy(page, &data, sizeof(data));

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, page, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

void persist_update(hendl_t *h) {
    bool changed = false;
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if ((uint8_t)h->ch[i].mode    != s_last_mode[i] ||
            (uint8_t)h->ch[i].op_mode != s_last_op[i]) {
            changed = true;
        }
    }

    if (changed) {
        for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
            s_last_mode[i] = (uint8_t)h->ch[i].mode;
            s_last_op[i]   = (uint8_t)h->ch[i].op_mode;
        }
        s_change_time_us = time_us_64();
        s_dirty          = true;
    }

    if (s_dirty) {
        uint64_t now = time_us_64();
        if ((now - s_change_time_us) >= ((uint64_t)PERSIST_SETTLE_MS * 1000u)) {
            persist_save(h);
            s_dirty = false;
        }
    }
}
