#include "channel.h"
#include "trigger.h"
#include "lfo.h"
#include "envelope.h"
#include "sample_hold.h"
#include "turing.h"
#include "euclidean.h"
#include "smooth_random.h"

// ============================================================
//  Channel DSP dispatcher
// ============================================================

void channel_init_all(hendl_t *h) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        channel_t *c = &h->ch[i];
        c->id      = i;
        c->mode    = MODE_LFO;
        c->op_mode = OP_LOOPING;
        c->output  = 0.0f;

        lfo_init(&c->lfo);
        envelope_init(&c->env);
        sample_hold_init(&c->sh);
    }
    turing_init(&h->ch[0].turing, &h->prng_state);
    euclidean_init(&h->ch[1].euclidean);
    smooth_random_init(&h->ch[2].smooth_rand, &h->prng_state);
}

// ------------------------------------------------------------
//  Process one DSP tick for a single channel.
//  The "unique" mode (index 3) differs per channel.
// ------------------------------------------------------------
static float process_channel(channel_t *c, uint32_t *prng) {
    channel_adcs_t *adcs = &c->adcs;
    bool trig            = c->trig.triggered;

    switch (c->mode) {
        case MODE_LFO:
            return lfo_process(&c->lfo, adcs, trig, c->op_mode);

        case MODE_ENVELOPE:
            return envelope_process(&c->env, adcs, trig);

        case MODE_SAMPLE_HOLD:
            return sample_hold_process(&c->sh, adcs, trig, c->op_mode, prng);

        case MODE_UNIQUE:
            switch (c->id) {
                case 0:
                    return turing_process(&c->turing, adcs, trig, c->op_mode, prng);
                case 1:
                    return euclidean_process(&c->euclidean, adcs, trig, c->op_mode, prng);
                case 2:
                    return smooth_random_process(&c->smooth_rand, adcs, trig, c->op_mode, prng);
            }
            break;
    }
    return 0.0f;
}

void channel_process_all(hendl_t *h) {
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        h->ch[i].output = process_channel(&h->ch[i], &h->prng_state);
        // Consume trigger after DSP has seen it
        trigger_consume(&h->ch[i].trig);
    }
}
