#pragma once
#include "types.h"

// ============================================================
//  ADC / MUX driver
//  Drives CD4051 address lines and reads ADC0 (mux) + ADC1
// ============================================================

void    adc_mux_init(void);

// Read one mux channel (0–7).  Selects address, waits settling,
// returns raw 12-bit ADC result.
uint16_t adc_mux_read(uint8_t mux_ch);

// Read direct ADC1 (CH3 CV)
uint16_t adc_direct_read(void);

// Convenience: populate all three channels' ADC structs.
// Called once per DSP tick.
void    adc_scan_all(channel_t ch[NUM_CHANNELS]);
