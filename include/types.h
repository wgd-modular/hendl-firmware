#pragma once

#include "config.h"

// ============================================================
//  Shared types for hendl firmware
// ============================================================

// ------------------------------------------------------------
//  Channel mode indices (same for all channels; unique modes
//  per channel are at index 3 but share the slot)
// ------------------------------------------------------------
typedef enum {
    MODE_LFO           = 0,
    MODE_ENVELOPE      = 1,
    MODE_SAMPLE_HOLD   = 2,
    MODE_UNIQUE        = 3,   // Turing / Euclidean / SmoothRandom per channel
} channel_mode_t;

// ------------------------------------------------------------
//  Loop vs trigger operating flag (toggled by long press)
// ------------------------------------------------------------
typedef enum {
    OP_LOOPING   = 0,
    OP_TRIGGERED = 1,
} op_mode_t;

// ------------------------------------------------------------
//  ADC snapshot — read once per loop tick, shared by all DSP
// ------------------------------------------------------------
typedef struct {
    float pot1;     // 0.0–1.0
    float pot2;     // 0.0–1.0
    float cv;       // 0.0–1.0 (or centred −0.5..+0.5 depending on mode)
    bool  cv_patched;
} channel_adcs_t;

// ------------------------------------------------------------
//  LFO state
// ------------------------------------------------------------
typedef enum {
    WAVE_SINE = 0,
    WAVE_TRIANGLE,
    WAVE_SAW_UP,
    WAVE_SAW_DOWN,
    WAVE_SQUARE,
    WAVE_COUNT,
} lfo_wave_t;

typedef struct {
    float       phase;          // 0.0–1.0
} lfo_state_t;

// ------------------------------------------------------------
//  Envelope (AD) state
// ------------------------------------------------------------
typedef enum {
    ENV_IDLE = 0,
    ENV_ATTACK,
    ENV_DECAY,
} env_stage_t;

typedef struct {
    env_stage_t stage;
    float       level;          // current output 0.0–1.0
    float       phase;          // stage phase accumulator 0.0–1.0
} env_state_t;

// ------------------------------------------------------------
//  Sample & Hold state
// ------------------------------------------------------------
typedef struct {
    float       held_value;     // output 0.0–1.0
    float       clock_phase;    // internal clock 0.0–1.0
    bool        last_clk_high;  // edge detect for internal clock
} sh_state_t;

// ------------------------------------------------------------
//  Turing Machine state (channel 1 unique mode)
// ------------------------------------------------------------
typedef struct {
    uint16_t    shift_reg;      // 16-bit shift register
    uint8_t     length;         // active loop length 2–16
    float       clock_phase;    // internal clock accumulator
} turing_state_t;

// ------------------------------------------------------------
//  Euclidean Sequencer state (channel 2 unique mode)
// ------------------------------------------------------------
typedef struct {
    uint8_t     pattern[EUCLIDEAN_MAX_STEPS];  // 1 = hit, 0 = rest
    uint8_t     length;         // total steps
    uint8_t     fill;           // hits
    uint8_t     rotation;       // offset
    uint8_t     step;           // current position
    uint32_t    trig_timer_us;  // countdown for pulse length
    bool        output_high;    // trigger output active
    float       clock_phase;    // internal clock accumulator
} euclidean_state_t;

// ------------------------------------------------------------
//  Smooth Random Generator state (channel 3 unique mode)
// ------------------------------------------------------------
typedef struct {
    float       current;        // current smoothed value 0.0–1.0
    float       target;         // target value
    float       clock_phase;    // update clock phase
} smooth_rand_state_t;

// ------------------------------------------------------------
//  Trigger / button I/O state (per channel)
// ------------------------------------------------------------
typedef struct {
    bool        last_raw;           // last raw GPIO level
    bool        triggered;          // rising-edge flag (consumed by DSP)
    uint64_t    last_edge_us;       // timestamp for debounce
} trigger_state_t;

typedef struct {
    bool        last_raw;
    bool        pressed_event;      // consumed single-press event
    bool        long_press_event;   // consumed long-press event
    uint64_t    press_start_us;
    uint64_t    last_edge_us;       // last stable transition timestamp
    bool        held;
    bool        long_fired;
} button_state_t;

// ------------------------------------------------------------
//  Normalization probe state (per CV input)
// ------------------------------------------------------------
typedef struct {
    uint32_t    lfsr_state;
    uint8_t     mismatch_count;
    uint8_t     window_idx;
    bool        patched;
} norm_state_t;

// ------------------------------------------------------------
//  Full channel state
// ------------------------------------------------------------
typedef struct {
    uint8_t             id;                 // 0, 1, 2
    channel_mode_t      mode;
    op_mode_t           op_mode;
    float               output;             // final output 0.0–1.0

    channel_adcs_t      adcs;
    trigger_state_t     trig;
    button_state_t      btn;
    norm_state_t        norm;

    // DSP sub-states (union would work but explicit is clearer)
    lfo_state_t         lfo;
    env_state_t         env;
    sh_state_t          sh;
    turing_state_t      turing;
    euclidean_state_t   euclidean;
    smooth_rand_state_t smooth_rand;
} channel_t;

// ------------------------------------------------------------
//  Global firmware context
// ------------------------------------------------------------
typedef struct {
    channel_t   ch[NUM_CHANNELS];
    uint8_t     sr_byte;            // current shift register LED state
    uint32_t    tick;               // DSP loop tick counter
    uint32_t    prng_state;         // global PRNG (LCG)
} hendl_t;

// ------------------------------------------------------------
//  Simple LCG PRNG (shared, inline)
// ------------------------------------------------------------
static inline uint32_t prng_next(uint32_t *state) {
    *state = 1103515245u * (*state) + 12345u;
    return *state;
}

static inline float prng_float(uint32_t *state) {
    return (float)(prng_next(state) & 0x7FFFu) / (float)0x7FFFu;
}
