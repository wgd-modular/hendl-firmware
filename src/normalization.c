#include "normalization.h"
#include "config.h"
#include "hardware/gpio.h"

#define PROBE_TOGGLE_TICKS  40u    // 2ms pro Phase bei 20kHz
#define PROBE_SETTLE_TICKS  4u

#define PROBE_SWING_THRESH  0.10f

static bool     s_probe_high   = false;
static uint32_t s_probe_tick   = 0u;
static uint32_t s_settle_ticks = 0u;
static bool     s_settled      = false;

static float    s_adc_high[NUM_CHANNELS];
static float    s_adc_low[NUM_CHANNELS];

void norm_init(hendl_t *h) {
    gpio_init(NORM_PROBE_PIN);
    gpio_set_dir(NORM_PROBE_PIN, GPIO_OUT);
    gpio_put(NORM_PROBE_PIN, 0);

    s_probe_high   = false;
    s_probe_tick   = 0u;
    s_settle_ticks = 0u;
    s_settled      = false;

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        s_adc_high[i] = 0.5f;
        s_adc_low[i]  = 0.5f;
        h->ch[i].norm.mismatch_count = 0;
        h->ch[i].norm.window_idx     = 0;
        h->ch[i].norm.patched        = false;
    }
}

void norm_update(hendl_t *h) {
    s_probe_tick++;
    if (s_probe_tick >= PROBE_TOGGLE_TICKS) {
        s_probe_tick   = 0u;
        s_probe_high   = !s_probe_high;
        gpio_put(NORM_PROBE_PIN, s_probe_high ? 1u : 0u);
        s_settled      = false;
        s_settle_ticks = 0u;
    }

    if (!s_settled) {
        s_settle_ticks++;
        if (s_settle_ticks >= PROBE_SETTLE_TICKS) {
            s_settled = true;
        }
        return;
    }

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        float adc_val = h->ch[i].adcs.cv;
        if (s_probe_high) {
            s_adc_high[i] = adc_val;
        } else {
            s_adc_low[i]  = adc_val;
        }

        float swing = s_adc_high[i] - s_adc_low[i];
        if (swing < 0.0f) swing = -swing;

        norm_state_t *n = &h->ch[i].norm;

        if (swing < PROBE_SWING_THRESH) {
            if (n->mismatch_count < NORM_WINDOW) n->mismatch_count++;
        } else {
            if (n->mismatch_count > 0u) n->mismatch_count--;
        }

        n->window_idx++;
        if (n->window_idx >= NORM_WINDOW) {
            n->patched    = (n->mismatch_count > NORM_MISMATCH_THRESH);
            n->window_idx = 0u;
        }
    }
}
