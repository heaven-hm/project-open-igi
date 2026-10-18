// Retail runtime observation: Level_Load - 0x004F0D40 - 2 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Catalog alias Task_New_2; working label Level_Load follows its retail error string. Script path and rate observed.
#pragma once
#include <stdint.h>
/* 004F0D40 Task_New_2 | gate PASS | openigi True */
int __convention Task_New_2("regparm")(int context, int arg2, int arg3, void * arg4, int arg5);
