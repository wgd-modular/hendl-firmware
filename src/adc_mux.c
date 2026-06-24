#include "adc_mux.h"
#include "config.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/time.h"

// ============================================================
//  ADC / MUX driver implementation
// ============================================================

// MUX settling: 2 µs after address change before sampling.
// At 20 kHz loop and small number of mux reads per tick this is safe.
#define MUX_SETTLE_US   2u

static void mux_select(uint8_t ch) {
    gpio_put(MUX_A_PIN, (ch >> 0) & 1u);
    gpio_put(MUX_B_PIN, (ch >> 1) & 1u);
    gpio_put(MUX_C_PIN, (ch >> 2) & 1u);
    busy_wait_us_32(MUX_SETTLE_US);
}

void adc_mux_init(void) {
    // Mux address GPIOs
    gpio_init(MUX_A_PIN);  gpio_set_dir(MUX_A_PIN, GPIO_OUT);
    gpio_init(MUX_B_PIN);  gpio_set_dir(MUX_B_PIN, GPIO_OUT);
    gpio_init(MUX_C_PIN);  gpio_set_dir(MUX_C_PIN, GPIO_OUT);
    gpio_put(MUX_A_PIN, 0);
    gpio_put(MUX_B_PIN, 0);
    gpio_put(MUX_C_PIN, 0);

    // ADC init
    adc_init();

    // ADC0 = GPIO26 (mux output)
    adc_gpio_init(ADC_MUX_PIN);

    // ADC1 = GPIO27 (CH3 CV direct)
    adc_gpio_init(ADC_CV3_PIN);
}

uint16_t adc_mux_read(uint8_t mux_ch) {
    mux_select(mux_ch);
    adc_select_input(0);  // ADC0 = GPIO26
    // Discard one sample after mux switch (charge injection)
    adc_read();
    return adc_read();
}

uint16_t adc_direct_read(void) {
    adc_select_input(1);  // ADC1 = GPIO27
    return adc_read();
}

void adc_scan_all(channel_t ch[NUM_CHANNELS]) {
    static const uint8_t pot_mux[6] = {
        MUX_CH1_POT1, MUX_CH1_POT2,
        MUX_CH2_POT1, MUX_CH2_POT2,
        MUX_CH3_POT1, MUX_CH3_POT2,
    };
    static uint8_t pot_idx = 0u;

    float pot = ADC_TO_FLOAT(adc_mux_read(pot_mux[pot_idx]));
    uint8_t c = pot_idx >> 1;
    if (pot_idx & 1u) ch[c].adcs.pot2 = pot;
    else              ch[c].adcs.pot1 = pot;
    pot_idx = (uint8_t)((pot_idx + 1u) % 6u);

    ch[0].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_mux_read(MUX_CH1_CV));
    ch[1].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_mux_read(MUX_CH2_CV));
    ch[2].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_direct_read());

    ch[0].adcs.cv_patched = ch[0].norm.patched;
    ch[1].adcs.cv_patched = ch[1].norm.patched;
    ch[2].adcs.cv_patched = ch[2].norm.patched;
}
