#pragma once
#include "types.h"

// ============================================================
//  Normalization detection (Mutable Instruments style)
//
//  A probe signal (LCG sequence) is driven into each CV input
//  via a very high impedance path.  When no cable is inserted,
//  the ADC reading tracks the probe.  When a cable is patched,
//  the low-impedance source overrides the probe.
//
//  In this firmware the "probe" is a digital PRNG value written
//  to an unused GPIO (high-Z output) that feeds the CV input
//  resistor divider.  The ADC is then compared to the expected
//  probe value.  If they match within tolerance → unpatched.
//
//  NOTE: actual probe GPIO drive requires board-level support
//  (probe pin tied to CV input via ~100 kΩ).  The firmware
//  provides the decision logic regardless; probe_gpio_write()
//  is a no-op stub if the feature is not wired.
// ============================================================

void    norm_init(hendl_t *h);

// Update normalization state for all channels.
// Must be called once per DSP tick AFTER adc_scan_all().
void    norm_update(hendl_t *h);
