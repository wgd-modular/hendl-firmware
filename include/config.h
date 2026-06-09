#pragma once

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ============================================================
//  hendl — Eurorack Multi Modulation Source
//  Raspberry Pi Pico 2 firmware
//  Target: RP2350
// ============================================================

// ------------------------------------------------------------
//  System timing
// ------------------------------------------------------------
#define SAMPLE_RATE_HZ          20000u          // Main DSP loop rate
#define SAMPLE_PERIOD_US        (1000000u / SAMPLE_RATE_HZ)
#define DEBOUNCE_MS             20u
#define LONG_PRESS_MS           800u

// ------------------------------------------------------------
//  PWM output
//  PWM wrap = 2047 → 11-bit resolution
//  PWM freq = sys_clk / (divider * wrap) = 150 MHz / (1 * 2048) ≈ 73.2 kHz
//  sys_clk = 150 MHz (Pico 2 default)
// ------------------------------------------------------------
#define PWM_WRAP                2047u
#define PWM_CLK_DIV_INT         1u
#define PWM_CLK_DIV_FRAC        0u     // divider = 1.0 → 73.2 kHz, 11-bit

// Internal 0.0–1.0 float maps to 0–4095 PWM counts
// Output stage: ×1.51 gain → 0–5 V from 0–3.3 V rail
#define PWM_FULL_SCALE          ((float)PWM_WRAP)

// ------------------------------------------------------------
//  ADC
//  12-bit ADC: 0–4095 counts = 0–3.3 V
// ------------------------------------------------------------
#define ADC_FULL_SCALE          4095.0f
#define ADC_TO_FLOAT(x)         ((float)(x) / ADC_FULL_SCALE)

// ------------------------------------------------------------
//  GPIO — Trigger inputs
// ------------------------------------------------------------
#define TRIG_CH1_PIN            12u
#define TRIG_CH2_PIN            13u
#define TRIG_CH3_PIN            14u

// ------------------------------------------------------------
//  GPIO — Mode buttons (active LOW, internal pull-up)
// ------------------------------------------------------------
#define BTN_CH1_PIN             19u
#define BTN_CH2_PIN             20u
#define BTN_CH3_PIN             18u

// ------------------------------------------------------------
//  GPIO — PWM CV outputs
// ------------------------------------------------------------
#define PWM_CH1_PIN             9u
#define PWM_CH2_PIN             10u
#define PWM_CH3_PIN             11u

// ------------------------------------------------------------
//  GPIO — 74HC595 shift register (mode LEDs)
// ------------------------------------------------------------
#define SR_SRCLK_PIN            2u   // Shift clock
#define SR_SER_PIN              3u   // Serial data
#define SR_RCLK_PIN             5u   // Latch clock

// ------------------------------------------------------------
//  GPIO — CD4051 multiplexer address lines
// ------------------------------------------------------------
#define MUX_A_PIN               6u
#define MUX_B_PIN               7u
#define MUX_C_PIN               8u

// ------------------------------------------------------------
//  ADC input pins
// ------------------------------------------------------------
#define ADC_MUX_PIN             26u  // GPIO26 = ADC0 — feeds CD4051 output
#define ADC_CV3_PIN             27u  // GPIO27 = ADC1 — CH3 CV direct

// ------------------------------------------------------------
//  CD4051 channel map
// ------------------------------------------------------------
#define MUX_CH1_POT1            0u // Correct
#define MUX_CH1_POT2            2u

#define MUX_CH2_POT1            4u
#define MUX_CH2_POT2            1u

#define MUX_CH3_POT1            3u
#define MUX_CH3_POT2            5u

#define MUX_CH1_CV              6u
#define MUX_CH2_CV              7u

// ------------------------------------------------------------
//  74HC595 LED bit positions (Q0=LSB, Q7=MSB)
//  Active HIGH
//  CH3: QB=bit1, QA=bit0
//  CH2: QD=bit3, QC=bit2
//  CH1: QF=bit5, QE=bit4
// ------------------------------------------------------------
#define LED_CH1_LED1_BIT        5u   // QF → D3
#define LED_CH1_LED2_BIT        4u   // QE → D4
#define LED_CH2_LED1_BIT        3u   // QD → D5
#define LED_CH2_LED2_BIT        2u   // QC → D6
#define LED_CH3_LED1_BIT        1u   // QB → D7
#define LED_CH3_LED2_BIT        0u   // QA → D8
// ------------------------------------------------------------
//  Channel / mode counts
// ------------------------------------------------------------
#define NUM_CHANNELS            3u
#define NUM_MODES               4u

// ------------------------------------------------------------
//  Normalization detection
//  LFSR probe: state = 1103515245 * state + 12345 (LCG)
//  Window size and mismatch threshold
// ------------------------------------------------------------
#define NORM_WINDOW             16u
#define NORM_MISMATCH_THRESH    2u
#define NORM_PROBE_TOLERANCE    0.10f

// ------------------------------------------------------------
//  Euclidean sequencer max steps
// ------------------------------------------------------------
#define EUCLIDEAN_MAX_STEPS     16u

// ------------------------------------------------------------
//  Turing Machine shift register width
// ------------------------------------------------------------
#define TURING_REGISTER_BITS    16u

// ------------------------------------------------------------
//  CV input scaling
//  Eurorack ±5 V scaled to 0–3.3 V ADC domain
//  0 V Eurorack → ~0.5 * 3.3 V = 1.65 V ADC midpoint
//  Unipolar use: 0–5 V Eurorack → 0–3.3 V (hardware passive divider)
// ------------------------------------------------------------
#define CV_BIPOLAR_OFFSET       0.5f    // subtract to centre-normalise

// ------------------------------------------------------------
// Normalization probe output pin
// ------------------------------------------------------------
#define NORM_PROBE_PIN          15u
