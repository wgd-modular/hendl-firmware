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

// ------------------------------------------------------------
//  Full ADC scan — reads all pots and CV inputs, updates
//  channel_adcs_t for each channel.
//  CV normalisation detection flag is set by normalization.c;
//  here we just capture the raw float value.
// ------------------------------------------------------------
void adc_scan_all(channel_t ch[NUM_CHANNELS]) {
    // Pot reads (via mux)
    ch[0].adcs.pot1 = ADC_TO_FLOAT(adc_mux_read(MUX_CH1_POT1));
    ch[0].adcs.pot2 = ADC_TO_FLOAT(adc_mux_read(MUX_CH1_POT2));
    ch[1].adcs.pot1 = ADC_TO_FLOAT(adc_mux_read(MUX_CH2_POT1));
    ch[1].adcs.pot2 = ADC_TO_FLOAT(adc_mux_read(MUX_CH2_POT2));
    ch[2].adcs.pot1 = ADC_TO_FLOAT(adc_mux_read(MUX_CH3_POT1));
    ch[2].adcs.pot2 = ADC_TO_FLOAT(adc_mux_read(MUX_CH3_POT2));

    // CV reads
    ch[0].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_mux_read(MUX_CH1_CV));
    ch[1].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_mux_read(MUX_CH2_CV));
    ch[2].adcs.cv = 1.0f - ADC_TO_FLOAT(adc_direct_read());

    // Patched flags set by normalization module; copy convenience flag
    ch[0].adcs.cv_patched = ch[0].norm.patched;
    ch[1].adcs.cv_patched = ch[1].norm.patched;
    ch[2].adcs.cv_patched = ch[2].norm.patched;
}
