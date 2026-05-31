#pragma once
#include "types.h"

// ============================================================
//  Mode button driver
//  Active LOW (GPIO pulled HIGH internally, button to GND).
//  Short press  → cycle mode 0→1→2→3→0
//  Long press   → toggle op_mode (looping ↔ triggered)
// ============================================================

void    button_init(void);

// Poll all buttons, update button_state_t for each channel.
// Must be called once per DSP tick.
void    button_poll(channel_t ch[NUM_CHANNELS]);
