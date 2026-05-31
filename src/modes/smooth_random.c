#include "smooth_random.h"
#include "config.h"
#include <math.h>

// ============================================================
//  Smooth Random Generator (Channel 3 only)
//  Continuous interpolated random walk (slewed noise).
//
//  Pot1: Smoothness — slew time (1 ms – 4 s)
//        Controls how fast the output chases the target.
//  Pot2: Change rate — how often a new target is drawn
//        (0.1 Hz – 10 Hz)
//  CV:   Bias / drift — offsets the target range (if patched)
//
//  Output: continuous 0.0–1.0, no discrete steps.
// ============================================================

#define SR_RATE_MIN     0.1f
#define SR_RATE_MAX     10.0f
#define SR_SLEW_MIN_S   0.001f
#define SR_SLEW_MAX_S   4.0f

static inline float pot_to_rate(float pot) {
    return SR_RATE_MIN * powf(SR_RATE_MAX / SR_RATE_MIN, pot);
}

static inline float pot_to_slew(float pot) {
    return SR_SLEW_MIN_S * powf(SR_SLEW_MAX_S / SR_SLEW_MIN_S, 1.0f - pot);
    // Inverted: pot=1 → fast slew (responsive), pot=0 → slow (smooth)
}

void smooth_random_init(smooth_rand_state_t *s, uint32_t *prng) {
    s->current     = prng_float(prng);
    s->target      = prng_float(prng);
    s->clock_phase = 0.0f;
}

float smooth_random_process(smooth_rand_state_t *s, const channel_adcs_t *adcs,
                            bool triggered, op_mode_t op_mode,
                            uint32_t *prng_state) {
    // New target rate
    float rate = pot_to_rate(adcs->pot2);

    bool new_target = false;
    if (op_mode == OP_TRIGGERED) {
        new_target = triggered;
    } else {
        float inc = rate / (float)SAMPLE_RATE_HZ;
        s->clock_phase += inc;
        if (s->clock_phase >= 1.0f) {
            s->clock_phase -= 1.0f;
            new_target = true;
        }
    }

    if (new_target) {
        float t = prng_float(prng_state);
        // CV bias: shift target distribution (if patched)
        if (adcs->cv_patched) {
            float bias = adcs->cv - 0.5f;   // −0.5..+0.5
            t = t * 0.7f + 0.15f + bias * 0.5f;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
        }
        s->target = t;
    }

    // One-pole low-pass slew: coefficient from slew time
    // coeff = e^(-1 / (slew_s * sample_rate))
    float slew_s = pot_to_slew(adcs->pot1);
    float coeff  = expf(-1.0f / (slew_s * (float)SAMPLE_RATE_HZ));

    s->current = s->current * coeff + s->target * (1.0f - coeff);

    return s->current;
}
