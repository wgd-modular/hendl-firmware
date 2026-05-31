#include "pwm_out.h"
#include "config.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

// ============================================================
//  PWM CV output driver
// ============================================================

static const uint8_t pwm_pins[NUM_CHANNELS] = {
    PWM_CH1_PIN,
    PWM_CH2_PIN,
    PWM_CH3_PIN,
};

void pwm_out_init(void) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        uint pin  = pwm_pins[i];
        uint slice = pwm_gpio_to_slice_num(pin);
        uint chan  = pwm_gpio_to_channel(pin);

        gpio_set_function(pin, GPIO_FUNC_PWM);

        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_wrap(&cfg, PWM_WRAP);
        pwm_config_set_clkdiv_int_frac(&cfg, PWM_CLK_DIV_INT, PWM_CLK_DIV_FRAC);
        pwm_init(slice, &cfg, true);

        // Start at 0
        pwm_set_chan_level(slice, chan, 0);
    }
}

void pwm_out_set(uint8_t ch_idx, float value) {
    if (ch_idx >= NUM_CHANNELS) return;

    // Clamp
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;

    uint16_t level = (uint16_t)(value * PWM_FULL_SCALE + 0.5f);
    if (level > PWM_WRAP) level = PWM_WRAP;

    uint pin   = pwm_pins[ch_idx];
    uint slice = pwm_gpio_to_slice_num(pin);
    uint chan  = pwm_gpio_to_channel(pin);
    pwm_set_chan_level(slice, chan, level);
}

void pwm_out_update_all(channel_t ch[NUM_CHANNELS]) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        pwm_out_set(i, ch[i].output);
    }
}
