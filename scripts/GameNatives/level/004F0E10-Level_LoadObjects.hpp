// Retail runtime observation: Level_LoadObjects - 0x004F0E10 - 2 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Level prefix and simulation rate 30 observed; formats objects.qsc before calling 0x004F0D40.
#pragma once
#include <stdint.h>
/* 004F0E10 Level_LoadObjects | gate PASS | openigi False */
int Level_LoadObjects(int context, int arg2);
