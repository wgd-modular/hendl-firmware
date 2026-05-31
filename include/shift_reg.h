#pragma once
#include "types.h"

// ============================================================
//  74HC595 shift register driver — mode LED output
//  Bit-banged SPI (no hardware SPI needed at LED update rate)
// ============================================================

void    shift_reg_init(void);

// Push one byte to the shift register and latch.
// Bit order: MSB first.
void    shift_reg_write(uint8_t data);

// Encode mode indices for all channels into an SR byte and write it.
void    shift_reg_update_leds(const channel_t ch[NUM_CHANNELS]);
