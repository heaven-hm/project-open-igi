// Native: DefineMagicObj; retail VA 0x004C4520; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004C4520.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Retail runtime observation: DefineMagicObj - 0x004C4520 - 381 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: QVM native callback observed; registry definition is distinct from placed MagicObj instances.
#pragma once
#include <stdint.h>
/* 004C4520 DefineMagicObj | gate PASS | openigi True */
int DefineMagicObj(int context, float arg2);
