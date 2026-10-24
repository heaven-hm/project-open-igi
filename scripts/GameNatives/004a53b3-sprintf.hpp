#pragma once
#include <stdint.h>

/* 0x004a53b3 sprintf | BN signature: int32_t(char* arg1, char* arg2) | readable identifiers, logic per 004a53b3-sprintf.cpp */
extern "C" int32_t sprintf(char* buffer, char* format);
