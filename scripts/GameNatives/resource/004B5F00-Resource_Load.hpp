// Retail runtime observation: Resource_Load - 0x004B5F00 - 2826 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Returns data pointer; optional second argument receives byte count. objects.qvm length 183749 = 0x2CDC5.
#pragma once
#include <stdint.h>
/* 004B5F00 Resource_Load | gate PASS | openigi True */
int* __convention Resource_Load("regparm")(int context, int arg2, void * arg3, int* arg4);
