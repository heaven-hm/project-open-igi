// Retail runtime observation: DynObj_LoadModelTextures - 0x004CF790 - 4 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Package texture traversal observed; texture Resource_Load requests originate at return address 0x004B24CE.
#pragma once
#include <stdint.h>
/* 004CF790 DynObj_LoadModelTextures | gate PASS | openigi True */
int* DynObj_LoadModelTextures(void * context);
