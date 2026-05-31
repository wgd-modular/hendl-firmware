#pragma once
#include "types.h"

void    lfo_init(lfo_state_t *s);
void    lfo_reset(lfo_state_t *s);
float   lfo_process(lfo_state_t *s, const channel_adcs_t *adcs, bool triggered, op_mode_t op_mode);
