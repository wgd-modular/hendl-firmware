#include "envelope.h"
#include "config.h"
#include <math.h>

// ============================================================
//  Envelope (AD) mode
//  Pot1: Attack  (1 ms – 4 s, log)
//  Pot2: Decay   (1 ms – 8 s, log)
//  CV:   Decay modulation (if patched)
//  Trigger: rising edge starts/re-triggers envelope
//  Output: exponential 0.0–1.0 unipolar
// ============================================================

#define ENV_ATTACK_MIN_S    0.001f
#define ENV_ATTACK_MAX_S    4.0f
#define ENV_DECAY_MIN_S     0.001f
#define ENV_DECAY_MAX_S     8.0f

// Exponential curve shape coefficient.
// Higher = more pronounced exponential.
#define ENV_EXP_K           12.0f

static inline float pot_to_time(float pot, float t_min, float t_max) {
    return t_min * powf(t_max / t_min, pot);
}

// Exponential mapping: produces a curve that rises quickly and slows down.
// phase 0→1 input, output 0→1 with exponential shape.
static inline float exp_curve(float phase) {
    // (e^(k*phase) - 1) / (e^k - 1)  normalised to 0–1
    static const float denom = 162753.79f;  // e^12 - 1, precomputed
    return (expf(ENV_EXP_K * phase) - 1.0f) / denom;
}

void envelope_init(env_state_t *s) {
    s->stage = ENV_IDLE;
    s->level = 0.0f;
    s->phase = 0.0f;
}

float envelope_process(env_state_t *s, const channel_adcs_t *adcs, bool triggered, op_mode_t op_mode) {
    // Retrigger on rising edge — restart attack from current level.
    // In looping mode also retrigger when decay naturally completes (handled below).
    if (triggered) {
        s->stage = ENV_ATTACK;
        s->phase = 0.0f;
        // Note: level is NOT reset — allows smooth retrigger from mid-decay
    }

    float attack_s = pot_to_time(adcs->pot1, ENV_ATTACK_MIN_S, ENV_ATTACK_MAX_S);
    float decay_s  = pot_to_time(adcs->pot2, ENV_DECAY_MIN_S,  ENV_DECAY_MAX_S);

    // CV modulates decay time (if patched): CV 0–1 scales decay 0.25×–4×
    if (adcs->cv_patched) {
        float cv_mod = adcs->cv * 2.0f - 0.5f;  // −0.5..+1.5 octave range
        decay_s *= powf(2.0f, cv_mod * 2.0f);
        if (decay_s < ENV_DECAY_MIN_S) decay_s = ENV_DECAY_MIN_S;
    }

    switch (s->stage) {
        case ENV_ATTACK: {
            float inc = 1.0f / ((float)SAMPLE_RATE_HZ * attack_s);
            s->phase += inc;
            if (s->phase >= 1.0f) {
                s->phase = 0.0f;
                s->level = 1.0f;
                s->stage = ENV_DECAY;
            } else {
                s->level = exp_curve(s->phase);
            }
            break;
        }
        case ENV_DECAY: {
            float inc = 1.0f / ((float)SAMPLE_RATE_HZ * decay_s);
            s->phase += inc;
            if (s->phase >= 1.0f) {
                s->phase = 0.0f;
                s->level = 0.0f;
                if (op_mode == OP_LOOPING) {
                    s->stage = ENV_ATTACK;
                } else {
                    s->stage = ENV_IDLE;
                }
            } else {
                // Decay = inverse exponential (1 → 0)
                s->level = 1.0f - exp_curve(s->phase);
            }
            break;
        }
        case ENV_IDLE:
        default:
            s->level = 0.0f;
            break;
    }

    return s->level;
}
