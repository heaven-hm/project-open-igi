#pragma once
#include <stdint.h>

/* 0x004c1790 QTaskList_Register | BN signature: int32_t*(int32_t* arg1, int32_t arg2) | readable identifiers, logic per 004c1790-QTaskList_Register.cpp */
extern "C" int32_t* QTaskList_Register(int32_t* task_list, int32_t task);
