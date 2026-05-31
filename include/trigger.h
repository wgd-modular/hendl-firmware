#pragma once
#include "types.h"

// ============================================================
//  Trigger input driver
//  GPIO polling with rising-edge detection.
//  Hardware: transistor conditioner → 3.3 V logic at GPIO.
//  Logic HIGH = active trigger.
// ============================================================

void    trigger_init(void);

// Poll all trigger GPIOs, update trigger_state_t for each channel.
// Must be called once per DSP tick.
// Sets trigger_state_t.triggered = true on rising edge (cleared after
// DSP consumes it via trigger_consume()).
void    trigger_poll(channel_t ch[NUM_CHANNELS]);

// Consume (clear) the trigger flag after DSP has acted on it.
static inline void trigger_consume(trigger_state_t *t) {
    t->triggered = false;
}
