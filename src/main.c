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

// ============================================================
//  hendl — main firmware entry point
//
//  Execution model: single-core polling loop on Core 0.
//  Precise timing via repeating alarm (hardware timer).
//  Core 1 is unused (available for future expansion).
//
//  Loop order per tick:
//    1. Poll trigger GPIOs
//    2. Poll mode buttons (debounce + mode/op change)
//    3. ADC scan (all pots + CVs via mux)
//    4. Normalization detection update
//    5. DSP: per-channel algorithm
//    6. PWM output update
//    7. LED update (shift register) — only when mode changes
// ============================================================

// Global firmware state — static to keep off stack
static hendl_t g_hendl;

// LED update needed flag — set when mode changes
static volatile bool g_led_dirty = true;

// ------------------------------------------------------------
//  DSP tick alarm callback
//  Called by hardware timer at SAMPLE_RATE_HZ.
// ------------------------------------------------------------
static volatile bool g_tick_flag = false;

static int64_t tick_alarm_callback(alarm_id_t id, void *user_data) {
    (void)id;
    (void)user_data;
    g_tick_flag = true;
    // Re-schedule: return negative value to repeat after |us| delay
    return -(int64_t)SAMPLE_PERIOD_US;
}

// ------------------------------------------------------------
//  Mode snapshot for LED dirty detection
// ------------------------------------------------------------
static uint8_t prev_modes[NUM_CHANNELS] = { 0xFF, 0xFF, 0xFF };

static void check_led_update(hendl_t *h) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if ((uint8_t)h->ch[i].mode != prev_modes[i]) {
            prev_modes[i] = (uint8_t)h->ch[i].mode;
            g_led_dirty = true;
        }
    }
}

// ------------------------------------------------------------
//  Initialise all hardware subsystems
// ------------------------------------------------------------
static void hw_init(hendl_t *h) {
    // Clock: Pico 2 defaults to 150 MHz; no explicit set needed.
    stdio_init_all();   // No-op if USB/UART stdio disabled in CMake

    adc_mux_init();
    pwm_out_init();
    shift_reg_init();
    trigger_init();
    button_init();

    // Firmware state
    h->tick        = 0u;
    h->prng_state  = 0xCAFEBABEu;
    h->sr_byte     = 0x00u;

    norm_init(h);
    channel_init_all(h);

    // Ensure silence on all outputs
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        pwm_out_set(i, 0.0f);
    }
}

// ------------------------------------------------------------
//  Main
// ------------------------------------------------------------
int main(void) {
    hw_init(&g_hendl);

    // Kick off periodic alarm
    alarm_id_t alarm = add_alarm_in_us(SAMPLE_PERIOD_US, tick_alarm_callback, NULL, true);
    (void)alarm;

    // Initial LED write
    shift_reg_update_leds(g_hendl.ch);

    while (true) {
        // Spin until alarm fires
        if (!g_tick_flag) {
            tight_loop_contents();  // Pico SDK low-power spin idiom
            continue;
        }
        g_tick_flag = false;

        // --- 1. Trigger polling ---
        trigger_poll(g_hendl.ch);

        // --- 2. Button polling (debounce + mode changes) ---
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

        // --- 7. LED update (only when mode changed) ---
        if (g_led_dirty) {
            shift_reg_update_leds(g_hendl.ch);
            g_led_dirty = false;
        }

        g_hendl.tick++;
    }

    return 0;
}
