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

void button_state_init(button_state_t *b, uint64_t now_us) {
    b->last_raw         = false;
    b->pressed_event    = false;
    b->long_press_event = false;
    b->press_start_us   = now_us;
    b->last_edge_us     = now_us;
    b->held             = false;
    b->long_fired       = false;
}

void button_poll(channel_t ch[NUM_CHANNELS]) {
    uint64_t now_us = time_us_64();

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        button_state_t *b = &ch[i].btn;
        bool raw_pressed  = !gpio_get(btn_pins[i]);

        bool stable = ((now_us - b->last_edge_us) >= ((uint64_t)DEBOUNCE_MS * 1000u));

        if (raw_pressed == b->last_raw) {
            if (b->held && !b->long_fired) {
                if ((now_us - b->press_start_us) >= ((uint64_t)LONG_PRESS_MS * 1000u)) {
                    b->long_press_event = true;
                    b->long_fired       = true;
                }
            }
        } else {
            if (stable) {
                b->last_raw     = raw_pressed;
                b->last_edge_us = now_us;

                if (raw_pressed) {
                    b->held           = true;
                    b->long_fired     = false;
                    b->press_start_us = now_us;
                } else {
                    if (b->held && !b->long_fired) {
                        b->pressed_event = true;
                    }
                    b->held       = false;
                    b->long_fired = false;
                }
            }
        }

        if (b->pressed_event) {
            b->pressed_event = false;
            ch[i].mode = (channel_mode_t)((ch[i].mode + 1) % NUM_MODES);
        }

        if (b->long_press_event) {
            b->long_press_event = false;
            ch[i].op_mode = (ch[i].op_mode == OP_LOOPING) ? OP_TRIGGERED : OP_LOOPING;
            ch[i].led_flash_pending = true;
        }
    }
}
