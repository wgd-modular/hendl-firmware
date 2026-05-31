#pragma once
#include "types.h"

void    turing_init(turing_state_t *s, uint32_t *prng);
float   turing_process(turing_state_t *s, const channel_adcs_t *adcs,
                       bool triggered, op_mode_t op_mode,
                       uint32_t *prng_state);
