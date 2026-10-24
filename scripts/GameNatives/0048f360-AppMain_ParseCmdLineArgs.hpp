#pragma once
#include <stdint.h>

/* 0x0048f360 AppMain_ParseCmdLineArgs | BN signature: void*(char* arg1, int32_t arg2) | readable identifiers, logic per 0048f360-AppMain_ParseCmdLineArgs.cpp */
extern "C" void* AppMain_ParseCmdLineArgs(char* prefix, int32_t callback);
