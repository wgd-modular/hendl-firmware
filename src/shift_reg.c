#include "shift_reg.h"
#include "config.h"
#include "hardware/gpio.h"

void shift_reg_init(void) {
    gpio_init(SR_SRCLK_PIN); gpio_set_dir(SR_SRCLK_PIN, GPIO_OUT); gpio_put(SR_SRCLK_PIN, 0);
    gpio_init(SR_SER_PIN);   gpio_set_dir(SR_SER_PIN,   GPIO_OUT); gpio_put(SR_SER_PIN,   0);
    gpio_init(SR_RCLK_PIN);  gpio_set_dir(SR_RCLK_PIN,  GPIO_OUT); gpio_put(SR_RCLK_PIN,  0);
    shift_reg_write(0x00);
}

void shift_reg_write(uint8_t data) {
    for (int8_t bit = 7; bit >= 0; bit--) {
        gpio_put(SR_SER_PIN, (data >> bit) & 0x01u);
        sleep_us(2);
        gpio_put(SR_SRCLK_PIN, 1);
        sleep_us(2);
        gpio_put(SR_SRCLK_PIN, 0);
        sleep_us(2);
    }
    sleep_us(2);
    gpio_put(SR_RCLK_PIN, 1);
    sleep_us(2);
    gpio_put(SR_RCLK_PIN, 0);
}

void shift_reg_update_leds(const channel_t ch[NUM_CHANNELS]) {
    uint8_t sr = 0;
    uint8_t mode, msb, lsb;

    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        mode = (uint8_t)ch[i].mode & 0x03u;
        msb  = (mode >> 1) & 0x01u;
        lsb  =  mode       & 0x01u;

        switch (i) {
            case 0:
                sr |= (msb << LED_CH1_LED1_BIT);
                sr |= (lsb << LED_CH1_LED2_BIT);
                break;
            case 1:
                sr |= (msb << LED_CH2_LED1_BIT);
                sr |= (lsb << LED_CH2_LED2_BIT);
                break;
            case 2:
                sr |= (msb << LED_CH3_LED1_BIT);
                sr |= (lsb << LED_CH3_LED2_BIT);
                break;
        }
    }

    shift_reg_write(sr);
}
