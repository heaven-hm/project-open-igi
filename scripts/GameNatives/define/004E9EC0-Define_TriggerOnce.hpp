#pragma once
#include <stdint.h>
// Native: DefineTriggerOnce; retail VA 0x004E9EC0; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004E9EC0.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: DefineTriggerOnce; retail VA 0x004E9EC0; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004E9EC0.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 004E9EC0 DefineTriggerOnce | gate PASS | openigi True */
int DefineTriggerOnce(long double context, long double arg2, long double arg3, long double arg4, long double arg5, long double arg6, long double arg7, int arg8, void * arg9, float arg10);
