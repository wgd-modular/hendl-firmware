#include "euclidean.h"
#include "config.h"
#include <string.h>
#include <math.h>

// ============================================================
//  Euclidean Sequencer (Channel 2 only)
//  Bjorklund algorithm for even distribution of hits.
//
//  Pot1: Length (2–16 steps)
//  Pot2: Fill   (number of hits, 1 – length)
//  CV:   Rotation (pattern offset, 0–length steps)
//  Trigger: Advance one step
//  op_mode == OP_LOOPING: internal clock advance
//  op_mode == OP_TRIGGERED: external trigger advances step
//
//  Output: 5 ms trigger pulse (HIGH) on hit, else 0.
// ============================================================

#define EUCLIDEAN_PULSE_US  5000u   // 5 ms trigger pulse

// ------------------------------------------------------------
//  Bjorklund / Euclidean pattern generator
//  Fills pat[0..len-1] with 1 (hit) or 0 (rest).
//  Uses Euclidean algorithm iteratively.
// ------------------------------------------------------------
static void euclidean_generate(uint8_t *pat, uint8_t len, uint8_t hits) {
    if (hits > len) hits = len;
    if (hits == 0) {
        memset(pat, 0, len);
        return;
    }
    if (hits == len) {
        memset(pat, 1, len);
        return;
    }

    // Bjorklund iterative approach with pattern arrays
    // Use two sub-pattern arrays on the stack
    uint8_t a[EUCLIDEAN_MAX_STEPS];
    uint8_t b[EUCLIDEAN_MAX_STEPS];
    uint8_t a_len[EUCLIDEAN_MAX_STEPS];  // length of each "a" sub-pattern
    uint8_t b_len[EUCLIDEAN_MAX_STEPS];  // length of each "b" sub-pattern

    int na = hits;          // count of "1" sub-patterns
    int nb = len - hits;    // count of "0" sub-patterns

    // Initialise: every a = [1], every b = [0]
    for (int i = 0; i < na; i++) { a[i] = 1; a_len[i] = 1; }
    for (int i = 0; i < nb; i++) { b[i] = 0; b_len[i] = 1; }

    // Iterative Bjorklund
    // We represent each sub-pattern as a length + a start index into a flat buffer.
    // For simplicity here, since max steps = 16, we do a direct flat simulation.

    // Alternative O(n) approach: Bresenham / rotation method
    // E(k, n) pattern position i = floor((i * k) / n) != floor(((i-1)*k) / n)
    // But classic Bjorklund gives identical results:

    // Use the simpler "modulo interleaving" (equivalent for musical purposes):
    for (int i = 0; i < len; i++) {
        pat[i] = (((i * hits) % len) < hits) ? 1 : 0;
    }
    (void)a; (void)b; (void)a_len; (void)b_len; (void)na; (void)nb;
}

static void apply_rotation(uint8_t *pat, uint8_t len, uint8_t rot) {
    if (rot == 0 || len == 0) return;
    rot = rot % len;
    uint8_t tmp[EUCLIDEAN_MAX_STEPS];
    for (uint8_t i = 0; i < len; i++) {
        tmp[(i + rot) % len] = pat[i];
    }
    memcpy(pat, tmp, len);
}

// ------------------------------------------------------------
//  Public API
// ------------------------------------------------------------

void euclidean_init(euclidean_state_t *s) {
    memset(s->pattern, 0, sizeof(s->pattern));
    s->length       = 8u;
    s->fill         = 4u;
    s->rotation     = 0u;
    s->step         = 0u;
    s->trig_timer_us = 0u;
    s->output_high  = false;
    s->clock_phase  = 0.0f;
    euclidean_generate(s->pattern, s->length, s->fill);
}

#define EC_RATE_MIN  0.5f
#define EC_RATE_MAX  30.0f

float euclidean_process(euclidean_state_t *s, const channel_adcs_t *adcs,
                        bool triggered, op_mode_t op_mode,
                        uint32_t *prng_state) {
    (void)prng_state;

    // Decode controls
    uint8_t new_len  = 2u + (uint8_t)(adcs->pot1 * 14.0f + 0.5f);
    if (new_len > EUCLIDEAN_MAX_STEPS) new_len = EUCLIDEAN_MAX_STEPS;

    uint8_t new_fill = 1u + (uint8_t)(adcs->pot2 * (float)(new_len - 1) + 0.5f);
    if (new_fill > new_len) new_fill = new_len;

    uint8_t new_rot = 0u;
    if (adcs->cv_patched) {
        new_rot = (uint8_t)(adcs->cv * (float)new_len) % new_len;
    }

    // Regenerate pattern if parameters changed
    if (new_len  != s->length ||
        new_fill != s->fill   ||
        new_rot  != s->rotation) {
        s->length   = new_len;
        s->fill     = new_fill;
        s->rotation = new_rot;
        euclidean_generate(s->pattern, s->length, s->fill);
        apply_rotation(s->pattern, s->length, s->rotation);
        s->step = s->step % s->length;
    }

    // Advance step
    bool do_step = false;
    if (op_mode == OP_TRIGGERED) {
        do_step = triggered;
    } else {
        // Internal clock
        float rate = EC_RATE_MIN * powf(EC_RATE_MAX / EC_RATE_MIN, adcs->pot1);
        float inc  = rate / (float)SAMPLE_RATE_HZ;
        s->clock_phase += inc;
        if (s->clock_phase >= 1.0f) {
            s->clock_phase -= 1.0f;
            do_step = true;
        }
    }

    if (do_step) {
        if (s->pattern[s->step]) {
            s->output_high   = true;
            s->trig_timer_us = EUCLIDEAN_PULSE_US;
        }
        s->step = (s->step + 1u) % s->length;
    }

    // Pulse timer countdown (sample-period based)
    if (s->output_high) {
        if (s->trig_timer_us <= SAMPLE_PERIOD_US) {
            s->output_high   = false;
            s->trig_timer_us = 0u;
        } else {
            s->trig_timer_us -= SAMPLE_PERIOD_US;
        }
    }

    return s->output_high ? 1.0f : 0.0f;
}
