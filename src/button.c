#include "button.h"
#include "config.h"
#include "hardware/gpio.h"
#include "pico/time.h"

static const uint8_t btn_pins[NUM_CHANNELS] = {
    BTN_CH1_PIN,
    BTN_CH2_PIN,
    BTN_CH3_PIN,
};

void button_init(void) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        gpio_init(btn_pins[i]);
        gpio_set_dir(btn_pins[i], GPIO_IN);
        gpio_pull_up(btn_pins[i]);
    }
}

void button_poll(channel_t ch[NUM_CHANNELS]) {
    uint64_t now_us = time_us_64();

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        button_state_t *b = &ch[i].btn;
        // Active LOW: GPIO LOW = pressed
        bool raw_pressed = !gpio_get(btn_pins[i]);

        // ---- Debounce: only act on stable transitions ----
        if (raw_pressed == b->last_raw) {
            // Stable state; check for long-press while held
            if (raw_pressed && b->held && !b->long_fired) {
                if ((now_us - b->press_start_us) >= (LONG_PRESS_MS * 1000u)) {
                    b->long_press_event = true;
                    b->long_fired = true;
                }
            }
        } else {
            // Transition: apply debounce window
            uint64_t delta_us = now_us - b->last_edge_us;
            if (delta_us >= (DEBOUNCE_MS * 1000u)) {
                b->last_raw      = raw_pressed;
                b->last_edge_us  = now_us;

                if (raw_pressed) {
                    // Press start
                    b->held           = true;
                    b->long_fired     = false;
                    b->press_start_us = now_us;
                } else {
                    // Release
                    if (b->held && !b->long_fired) {
                        // Short press — only fire if long press did NOT already fire
                        b->pressed_event = true;
                    }
                    b->held       = false;
                    b->long_fired = false;
                }
            }
        }

        // ---- Consume events: update channel state ----
        if (b->pressed_event) {
            b->pressed_event = false;
            // Cycle mode
            ch[i].mode = (channel_mode_t)((ch[i].mode + 1) % NUM_MODES);
        }

        if (b->long_press_event) {
            b->long_press_event = false;
            // Toggle operating mode
            ch[i].op_mode = (ch[i].op_mode == OP_LOOPING) ? OP_TRIGGERED : OP_LOOPING;
        }
    }
}
