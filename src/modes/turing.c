#include "turing.h"
#include "config.h"
#include <math.h>

// ============================================================
//  Turing Machine (Channel 1 only)
//  16-bit shift register random sequencer.
//
//  Pot1: Lock / Randomness
//        0.0 = fully locked loop (deterministic, repeats)
//        0.5 = 50% random (maximum chaos)
//        1.0 = fully locked inverse (deterministic, inverted)
//  Pot2: Loop length (2–16 steps)
//  CV:   Bias modulation on Pot1 (additive, if patched)
//
//  op_mode == OP_LOOPING   → internal clock at fixed rate
//  op_mode == OP_TRIGGERED → external trigger advances one step
//
//  Output: shift register upper bits → 0.0–1.0 unipolar
//          (no internal attenuation; use external VCA/attenuator)
// ============================================================

// Fixed internal clock rate in looping mode
#define TM_INTERNAL_RATE_HZ     8.0f    // ~120 BPM / 16th notes

void turing_init(turing_state_t *s, uint32_t *prng) {
    s->shift_reg   = (uint16_t)(prng_next(prng) & 0xFFFFu);
    if (s->shift_reg == 0) s->shift_reg = 0xACE1u;
    s->length      = 8u;
    s->clock_phase = 0.0f;
}

// Map upper `len` bits of register to 0.0–1.0
static float register_to_float(uint16_t reg, uint8_t len) {
    if (len == 0) return 0.0f;
    uint16_t mask = (len >= 16) ? 0xFFFFu : (uint16_t)((1u << len) - 1u);
    uint16_t val  = (reg >> (16u - len)) & mask;
    return (float)val / (float)mask;
}

float turing_process(turing_state_t *s, const channel_adcs_t *adcs,
                     bool triggered, op_mode_t op_mode,
                     uint32_t *prng_state) {

    // --- Pot2: Loop length 2–16 steps ---
    uint8_t new_len = 2u + (uint8_t)(adcs->pot2 * 14.0f + 0.5f);
    if (new_len > TURING_REGISTER_BITS) new_len = TURING_REGISTER_BITS;
    if (new_len < 2u) new_len = 2u;
    s->length = new_len;

    // --- Pot1 + CV: Randomness/lock probability ---
    float rand_prob = adcs->pot1;
    if (adcs->cv_patched) {
        // CV centred at 0.5; shift ±0.25 range onto rand_prob
        rand_prob += (adcs->cv - 0.5f) * 0.5f;
        if (rand_prob < 0.0f) rand_prob = 0.0f;
        if (rand_prob > 1.0f) rand_prob = 1.0f;
    }

    // --- Clock: internal fixed rate or external trigger ---
    bool do_step = false;

    if (op_mode == OP_TRIGGERED) {
        do_step = triggered;
    } else {
        float inc = TM_INTERNAL_RATE_HZ / (float)SAMPLE_RATE_HZ;
        s->clock_phase += inc;
        if (s->clock_phase >= 1.0f) {
            s->clock_phase -= 1.0f;
            do_step = true;
        }
    }

    // --- Shift register step ---
    if (do_step) {
        uint8_t  loop_msb_pos = 16u - s->length;
        uint16_t loop_mask    = (s->length >= 16u)
                                ? 0xFFFFu
                                : (uint16_t)((1u << s->length) - 1u);

        // MSB of active loop window — recycled when locked
        uint8_t msb_bit = (uint8_t)((s->shift_reg >> (TURING_REGISTER_BITS - 1u)) & 0x01u);

        // Decide new LSB to shift in
        uint8_t new_bit;
        float   chaos = 1.0f - fabsf(rand_prob * 2.0f - 1.0f);
        float   rnd   = prng_float(prng_state);

        if (rnd < chaos) {
            // Random bit
            new_bit = (uint8_t)(prng_float(prng_state) > 0.5f ? 1u : 0u);
        } else {
            // Lock: recycle MSB (rand_prob < 0.5) or its inverse (> 0.5)
            // Smooth transition: above 0.5 the "locked" value inverts
            new_bit = (rand_prob <= 0.5f) ? msb_bit : (msb_bit ^ 0x01u);
        }

        // Shift left within loop window, insert new_bit at LSB position
        uint16_t loop_section = (s->shift_reg >> loop_msb_pos) & loop_mask;
        loop_section = ((loop_section << 1u) | new_bit) & loop_mask;

        // Write loop section back, preserve bits outside window
        s->shift_reg  = (uint16_t)(s->shift_reg & ~(loop_mask << loop_msb_pos));
        s->shift_reg |= (uint16_t)(loop_section << loop_msb_pos);
    }

    // Output: upper `length` bits → 0.0–1.0 (full scale, no attenuation)
    return register_to_float(s->shift_reg, s->length);
}
