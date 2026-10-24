#pragma once
#include <stdint.h>

/* 0x004b38e0 Real32x9_Create | BN signature: int32_t(float* arg1, float arg2, float arg3, float arg4) | readable identifiers, logic per 004b38e0-Real32x9_Create.cpp */
extern "C" int32_t Real32x9_Create(float* matrix, float angle_x, float angle_y, float angle_z);
