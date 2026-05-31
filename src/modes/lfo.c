#include "lfo.h"
#include "config.h"
#include <math.h>

// ============================================================
//  LFO mode
//  Pot1: Rate   (logarithmic: ~0.05 Hz – 20 Hz)
//  Pot2: Waveform (5 continuous shapes, blended at boundaries)
//  CV:   Rate modulation (additive, if patched)
//  Output: unipolar 0.0–1.0
// ============================================================

// Rate range: 0.05 Hz (slow) to 20 Hz (fast)
#define LFO_RATE_MIN    0.05f
#define LFO_RATE_MAX    20.0f

// Map pot value (0–1) to Hz logarithmically
static inline float pot_to_rate(float pot) {
    // log-taper: rate = MIN * (MAX/MIN)^pot
    return LFO_RATE_MIN * powf(LFO_RATE_MAX / LFO_RATE_MIN, pot);
}

// Waveform generators — all accept phase 0.0–1.0, return 0.0–1.0
static inline float wave_sine(float p)      { return 0.5f + 0.5f * sinf(p * 6.28318530f); }
static inline float wave_triangle(float p)  { return (p < 0.5f) ? (p * 2.0f) : (2.0f - p * 2.0f); }
static inline float wave_saw_up(float p)    { return p; }
static inline float wave_saw_down(float p)  { return 1.0f - p; }
static inline float wave_square(float p)    { return (p < 0.5f) ? 1.0f : 0.0f; }

// Pot2 spans 0–1 across 5 waveforms.
// Each waveform occupies a 0.2 segment; blending occurs in
// a ±0.025 overlap region at each boundary.
static float blend_waveforms(float pot2, float phase) {
    float pos   = pot2 * (WAVE_COUNT - 1);  // 0.0 – 4.0
    int   idx   = (int)pos;
    float frac  = pos - (float)idx;

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

    // Linear crossfade between adjacent waveforms
    return waveforms[idx] * (1.0f - frac) + waveforms[idx + 1] * frac;
}

void lfo_init(lfo_state_t *s) {
    s->phase = 0.0f;
}

void lfo_reset(lfo_state_t *s) {
    s->phase = 0.0f;
}

float lfo_process(lfo_state_t *s, const channel_adcs_t *adcs, bool triggered, op_mode_t op_mode) {
    // Rate: base from Pot1, modulated by CV if patched
    float rate = pot_to_rate(adcs->pot1);
    if (adcs->cv_patched) {
        // CV centred at 0.5; shift ±half-rate
        float cv_mod = (adcs->cv - 0.5f) * 2.0f;   // −1..+1
        rate *= (1.0f + cv_mod);
        if (rate < 0.0001f) rate = 0.0001f;
    }

    // Phase increment per sample
    float inc = rate / (float)SAMPLE_RATE_HZ;

    // In triggered mode, reset phase on incoming trigger
    if (op_mode == OP_TRIGGERED && triggered) {
        s->phase = 0.0f;
    }

    s->phase += inc;
    if (s->phase >= 1.0f) s->phase -= 1.0f;

    return blend_waveforms(adcs->pot2, s->phase);
}
