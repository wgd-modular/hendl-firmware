#pragma once
#include "types.h"

// ============================================================
//  PWM CV output driver
//  Three independent PWM channels, one per CV output pin.
//  Resolution: 12-bit (0–4095)
//  Downstream: RC filter + ×1.51 op-amp stage → 0–5 V
// ============================================================

void    pwm_out_init(void);

// Write normalised float (0.0–1.0) to a channel's PWM.
// Values are clamped.
void    pwm_out_set(uint8_t ch_idx, float value);

// Convenience: write all three channels from array
void    pwm_out_update_all(channel_t ch[NUM_CHANNELS]);
