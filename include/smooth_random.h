#pragma once
#include "types.h"

void    smooth_random_init(smooth_rand_state_t *s, uint32_t *prng);
float   smooth_random_process(smooth_rand_state_t *s, const channel_adcs_t *adcs,
                              bool triggered, op_mode_t op_mode,
                              uint32_t *prng_state);
