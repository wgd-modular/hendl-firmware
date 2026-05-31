#pragma once
#include "types.h"

void    sample_hold_init(sh_state_t *s);
float   sample_hold_process(sh_state_t *s, const channel_adcs_t *adcs,
                            bool triggered, op_mode_t op_mode,
                            uint32_t *prng_state);
