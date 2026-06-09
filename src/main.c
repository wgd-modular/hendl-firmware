#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#include "config.h"
#include "types.h"
#include "adc_mux.h"
#include "pwm_out.h"
#include "shift_reg.h"
#include "trigger.h"
#include "button.h"
#include "normalization.h"
#include "channel.h"

static hendl_t g_hendl;
static volatile bool g_led_dirty = true;
static volatile bool g_tick_flag = false;
static uint8_t prev_modes[NUM_CHANNELS] = { 0xFF, 0xFF, 0xFF };

static int64_t tick_alarm_callback(alarm_id_t id, void *user_data) {
    (void)id;
    (void)user_data;
    g_tick_flag = true;
    return -(int64_t)SAMPLE_PERIOD_US;
}

static void check_led_update(hendl_t *h) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if ((uint8_t)h->ch[i].mode != prev_modes[i]) {
            prev_modes[i] = (uint8_t)h->ch[i].mode;
            g_led_dirty = true;
        }
    }
}

// Berechne SR-Byte für alle Kanäle außer `skip_ch`
static uint8_t sr_without_channel(const hendl_t *h, uint8_t skip_ch) {
    uint8_t sr = 0x00u;
    for (uint8_t j = 0; j < NUM_CHANNELS; j++) {
        if (j == skip_ch) continue;
        uint8_t m   = (uint8_t)h->ch[j].mode & 0x03u;
        uint8_t msb = (m >> 1) & 0x01u;
        uint8_t lsb =  m       & 0x01u;
        switch (j) {
            case 0: sr |= (msb << LED_CH1_LED1_BIT) | (lsb << LED_CH1_LED2_BIT); break;
            case 1: sr |= (msb << LED_CH2_LED1_BIT) | (lsb << LED_CH2_LED2_BIT); break;
            case 2: sr |= (msb << LED_CH3_LED1_BIT) | (lsb << LED_CH3_LED2_BIT); break;
        }
    }
    return sr;
}

static void do_double_flash(const hendl_t *h, uint8_t ch_idx) {
    uint8_t ch_mask = 0x00u;
    switch (ch_idx) {
        case 0: ch_mask = (1u << LED_CH1_LED1_BIT) | (1u << LED_CH1_LED2_BIT); break;
        case 1: ch_mask = (1u << LED_CH2_LED1_BIT) | (1u << LED_CH2_LED2_BIT); break;
        case 2: ch_mask = (1u << LED_CH3_LED1_BIT) | (1u << LED_CH3_LED2_BIT); break;
    }
    uint8_t sr_rest = sr_without_channel(h, ch_idx);

    shift_reg_write(sr_rest);
    sleep_ms(80);
    shift_reg_write(sr_rest | ch_mask);
    sleep_ms(80);
    shift_reg_write(sr_rest);
    sleep_ms(80);
    shift_reg_write(sr_rest | ch_mask);
    sleep_ms(80);
    shift_reg_update_leds(h->ch);
}

static void hw_init(hendl_t *h) {
    stdio_init_all();
    adc_mux_init();
    pwm_out_init();
    shift_reg_init();
    trigger_init();
    button_init();

    h->tick       = 0u;
    h->prng_state = 0xCAFEBABEu;
    h->sr_byte    = 0x00u;

    norm_init(h);
    channel_init_all(h);

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        pwm_out_set(i, 0.0f);
    }
}

int main(void) {
    hw_init(&g_hendl);

    alarm_id_t alarm = add_alarm_in_us(SAMPLE_PERIOD_US, tick_alarm_callback, NULL, true);
    (void)alarm;

    shift_reg_update_leds(g_hendl.ch);

    while (true) {
        if (!g_tick_flag) {
            tight_loop_contents();
            continue;
        }
        g_tick_flag = false;

        // --- 1. Trigger polling ---
        trigger_poll(g_hendl.ch);

        // --- 2. Button polling ---
        button_poll(g_hendl.ch);
        check_led_update(&g_hendl);

        // --- 3. ADC scan ---
        adc_scan_all(g_hendl.ch);

        // --- 4. Normalization update ---
        norm_update(&g_hendl);

        // --- 5. DSP ---
        channel_process_all(&g_hendl);

        // --- 6. PWM output ---
        pwm_out_update_all(g_hendl.ch);

        // --- 7. LED update ---
        if (g_led_dirty) {
            shift_reg_update_leds(g_hendl.ch);
            g_led_dirty = false;
        }

        // --- 8. Double flash bei op_mode Toggle ---
        for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
            if (g_hendl.ch[i].led_flash_pending) {
                g_hendl.ch[i].led_flash_pending = false;
                do_double_flash(&g_hendl, i);
            }
        }

        g_hendl.tick++;
    }

    return 0;
}
