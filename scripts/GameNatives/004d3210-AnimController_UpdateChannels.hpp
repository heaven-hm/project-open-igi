#pragma once
#include <stdint.h>

/* 0x004d3210 AnimController_UpdateChannels | BN signature: int32_t(int32_t arg1, int32_t* arg2) | readable identifiers, logic per 004d3210-AnimController_UpdateChannels.cpp */
extern "C" int32_t AnimController_UpdateChannels(int32_t render_context, int32_t* channel_set);
