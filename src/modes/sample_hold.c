#include "sample_hold.h"
#include "config.h"
#include <math.h>

// ============================================================
//  Sample & Hold mode
//  Pot1: Internal clock rate (0.05 Hz – 30 Hz, log)
//  Pot2: Input level / gain (0–1 attenuation on CV)
//  CV:   Signal to be sampled
//
//  op_mode == OP_LOOPING  → internal clock
//  op_mode == OP_TRIGGERED → external trigger clocks S&H
//
//  No slew (hard S&H).
//  If CV unpatched, internal PRNG is used as noise source.
// ============================================================

#define SH_RATE_MIN     0.05f
#define SH_RATE_MAX     30.0f

static inline float pot_to_rate(float pot) {
    return SH_RATE_MIN * powf(SH_RATE_MAX / SH_RATE_MIN, pot);
}

void sample_hold_init(sh_state_t *s) {
    s->held_value    = 0.0f;
    s->clock_phase   = 0.0f;
    s->last_clk_high = false;
}

float sample_hold_process(sh_state_t *s, const channel_adcs_t *adcs,
                          bool triggered, op_mode_t op_mode,
                          uint32_t *prng_state) {
    bool do_sample = false;

    if (op_mode == OP_TRIGGERED) {
        // External trigger clocks S&H
        do_sample = triggered;
    } else {
        // Internal clock
        float rate = pot_to_rate(adcs->pot1);
        float inc  = rate / (float)SAMPLE_RATE_HZ;
        s->clock_phase += inc;
        if (s->clock_phase >= 1.0f) {
            s->clock_phase -= 1.0f;
            do_sample = true;
        }
    }

    if (do_sample) {
        float input;
        if (adcs->cv_patched) {
            input = adcs->cv;
        } else {
            // Unpatched: use internal PRNG noise
            input = prng_float(prng_state);
        }
        // Apply Pot2 level (attenuator)
        s->held_value = input * adcs->pot2;
    }

    return s->held_value;
}
