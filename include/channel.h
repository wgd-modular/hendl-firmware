#pragma once
#include "types.h"

// ============================================================
//  Channel DSP dispatcher
//  Calls the appropriate mode algorithm for each channel.
// ============================================================

void    channel_init_all(hendl_t *h);
void    channel_process_all(hendl_t *h);
