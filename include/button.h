#pragma once
#include "types.h"

void    button_init(void);
void    button_state_init(button_state_t *b, uint64_t now_us);
void    button_poll(channel_t ch[NUM_CHANNELS]);
