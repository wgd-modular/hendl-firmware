#include "lfo.h"
#include "config.h"
#include <math.h>

// ============================================================
//  LFO mode
//  OP_LOOPING: free-running, trigger resets phase, CV modulates rate
//    Pot1: Rate (log taper, 0.05–20 Hz)
//    Pot2: Waveform (5 shapes, blended)
//    CV:   Rate modulation (centred, ±1× rate)
//
//  OP_TRIGGERED (gate/clock-sync mode):
//    The LFO period is derived from the interval between rising edges.
//    Phase resets to 0 on each rising edge and advances continuously.
//    Pot1: Phase offset (0.0–1.0 → 0°–360°), added at output only
//    Pot2: Waveform selection
//    CV:   Waveform modulation (added to Pot2, clamped 0–1)
// ============================================================

#define LFO_RATE_MIN    0.05f
#define LFO_RATE_MAX    20.0f

static inline float pot_to_rate(float pot) {
    return LFO_RATE_MIN * powf(LFO_RATE_MAX / LFO_RATE_MIN, pot);
}

// Waveform generators — phase 0.0–1.0, return 0.0–1.0
static inline float wave_sine(float p)      { return 0.5f + 0.5f * sinf(p * 6.28318530f); }
static inline float wave_triangle(float p)  { return (p < 0.5f) ? (p * 2.0f) : (2.0f - p * 2.0f); }
static inline float wave_saw_up(float p)    { return p; }
static inline float wave_saw_down(float p)  { return 1.0f - p; }
static inline float wave_square(float p)    { return (p < 0.5f) ? 1.0f : 0.0f; }

static float blend_waveforms(float pot2, float phase) {
    float pos  = pot2 * (WAVE_COUNT - 1);  // 0.0 – 4.0
    int   idx  = (int)pos;
    float frac = pos - (float)idx;

    if (idx >= (WAVE_COUNT - 1)) {
        idx  = WAVE_COUNT - 2;
        frac = 1.0f;
    }

    float waveforms[WAVE_COUNT];
    waveforms[0] = wave_sine(phase);
    waveforms[1] = wave_triangle(phase);
    waveforms[2] = wave_saw_up(phase);
    waveforms[3] = wave_saw_down(phase);
    waveforms[4] = wave_square(phase);

    return waveforms[idx] * (1.0f - frac) + waveforms[idx + 1] * frac;
}

void lfo_init(lfo_state_t *s) {
    s->phase        = 0.0f;
    s->gate_timer   = 0;
    s->gate_period  = 0;
}

void lfo_reset(lfo_state_t *s) {
    s->phase        = 0.0f;
    s->gate_timer   = 0;
    s->gate_period  = 0;
}

float lfo_process(lfo_state_t *s, const channel_adcs_t *adcs, bool triggered, op_mode_t op_mode) {
    if (op_mode == OP_LOOPING) {
        float rate = pot_to_rate(adcs->pot1);
        if (adcs->cv_patched) {
            float cv_mod = (adcs->cv - 0.5f) * 2.0f;  // −1..+1
            rate *= (1.0f + cv_mod);
            if (rate < 0.0001f) rate = 0.0001f;
        }

        if (triggered) {
            s->phase = 0.0f;
        }

        s->phase += rate / (float)SAMPLE_RATE_HZ;
        if (s->phase >= 1.0f) s->phase -= 1.0f;

        return blend_waveforms(adcs->pot2, s->phase);
    } else {
        // Clock-sync mode: LFO period tracks the interval between rising edges.
        s->gate_timer++;

        if (triggered) {
            if (s->gate_timer > 0) {
                s->gate_period = s->gate_timer;
            }
            s->gate_timer = 0;
            s->phase      = 0.0f;
        }

        if (s->gate_period > 0) {
            s->phase += 1.0f / (float)s->gate_period;
            if (s->phase >= 1.0f) s->phase -= 1.0f;
        }

        // Pot1 is a phase offset applied at output only
        float output_phase = fmodf(s->phase + adcs->pot1, 1.0f);

        // CV modulates waveform selection
        float wave_sel = adcs->pot2;
        if (adcs->cv_patched) {
            wave_sel += (adcs->cv - 0.5f);
            if (wave_sel < 0.0f) wave_sel = 0.0f;
            if (wave_sel > 1.0f) wave_sel = 1.0f;
        }

        return blend_waveforms(wave_sel, output_phase);
    }
}
