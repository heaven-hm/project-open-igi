#pragma once
#include <stdint.h>

/* 0x004b0c60 Mem_AllocFn | BN signature: void*(int32_t arg1, int32_t arg2) | readable identifiers, logic per 004b0c60-Mem_AllocFn.cpp */
extern "C" void* Mem_AllocFn(int32_t num_bytes, int32_t alignment);
