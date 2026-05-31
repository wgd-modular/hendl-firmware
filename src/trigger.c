#include "trigger.h"
#include "config.h"
#include "hardware/gpio.h"

static const uint8_t trig_pins[NUM_CHANNELS] = {
    TRIG_CH1_PIN,
    TRIG_CH2_PIN,
    TRIG_CH3_PIN,
};

void trigger_init(void) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        gpio_init(trig_pins[i]);
        gpio_set_dir(trig_pins[i], GPIO_IN);
        // No internal pull-up: external transistor circuit provides defined levels
        gpio_disable_pulls(trig_pins[i]);
    }
}

void trigger_poll(channel_t ch[NUM_CHANNELS]) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        trigger_state_t *t = &ch[i].trig;
        bool raw = gpio_get(trig_pins[i]);

        // Rising edge = trigger event
        if (raw && !t->last_raw) {
            t->triggered = true;
        }
        t->last_raw = raw;
    }
}
