// Retail runtime observation: Qvm_Load - 0x004B80B0 - 157 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: The objects.qsc input selects objects.qvm through the compiled loader.
#pragma once
#include <stdint.h>
/* 004B80B0 Qvm_Load | gate PASS | openigi False */
int* Qvm_Load(void * context);
