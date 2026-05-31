#pragma once
#include "types.h"

void    euclidean_init(euclidean_state_t *s);
float   euclidean_process(euclidean_state_t *s, const channel_adcs_t *adcs,
                          bool triggered, op_mode_t op_mode,
                          uint32_t *prng_state);
