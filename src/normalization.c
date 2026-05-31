#include "normalization.h"
#include "config.h"

// ============================================================
//  Normalization detection
// ============================================================

// Per-channel expected probe level derived from LCG state
// mapped to 0.0–1.0 range (same as ADC float)
static inline float probe_to_float(uint32_t state) {
    return (float)(state & 0x7FFFu) / (float)0x7FFFu;
}

void norm_init(hendl_t *h) {
    // Seed each channel's LCG with distinct values
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        h->ch[i].norm.lfsr_state    = 0xDEAD0000u + (uint32_t)i * 0x1337u;
        h->ch[i].norm.mismatch_count = 0;
        h->ch[i].norm.window_idx     = 0;
        h->ch[i].norm.patched        = false;
    }
}

void norm_update(hendl_t *h) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        norm_state_t *n  = &h->ch[i].norm;
        float adc_val    = h->ch[i].adcs.cv;

        // Advance probe LCG
        n->lfsr_state = 1103515245u * n->lfsr_state + 12345u;
        float expected = probe_to_float(n->lfsr_state);

        // Compare ADC reading to expected probe level
        float diff = adc_val - expected;
        if (diff < 0.0f) diff = -diff;

        if (diff > NORM_PROBE_TOLERANCE) {
            n->mismatch_count++;
        }

        n->window_idx++;

        // Evaluate window
        if (n->window_idx >= NORM_WINDOW) {
            n->patched        = (n->mismatch_count > NORM_MISMATCH_THRESH);
            n->mismatch_count = 0;
            n->window_idx     = 0;
        }
    }
}
