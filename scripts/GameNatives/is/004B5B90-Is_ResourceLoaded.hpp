// Retail runtime observation: Is_ResourceLoaded - 0x004B5B90 - 314 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: QVM cache lookup entry observed; cache-hit branch semantics not dynamically isolated.
#pragma once
#include <stdint.h>
/* 004B5B90 Is_ResourceLoaded | gate PASS | openigi False */
int Is_ResourceLoaded(void * context, void ** arg2);
