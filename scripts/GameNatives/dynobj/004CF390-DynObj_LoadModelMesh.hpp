// Retail runtime observation: DynObj_LoadModelMesh - 0x004CF390 - 512 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Model name entries observed, capped at 512 by probe; 0x004CF35F is the catalogue caller return address.
#pragma once
#include <stdint.h>
/* 004CF390 DynObj_LoadModelMesh | gate PASS | openigi True */
int* DynObj_LoadModelMesh(int context);
