#pragma once
#include <stdint.h>

/* 0x004e76e0 TextWindow_vprintf | BN signature: int32_t(int32_t* arg1, char* arg2, int32_t arg3) | readable identifiers, logic per 004e76e0-TextWindow_vprintf.cpp */
extern "C" int32_t TextWindow_vprintf(int32_t* text_window, char* format, int32_t args);
