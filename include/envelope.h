#pragma once
#include "types.h"

void    envelope_init(env_state_t *s);
float   envelope_process(env_state_t *s, const channel_adcs_t *adcs, bool triggered);
