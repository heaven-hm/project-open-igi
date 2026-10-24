#pragma once
#include <stdint.h>

/* 0x004c1800 QTaskList_New | BN signature: int32_t*(int32_t arg1) | readable identifiers, logic per 004c1800-QTaskList_New.cpp */
extern "C" int32_t* QTaskList_New(int32_t capacity);
