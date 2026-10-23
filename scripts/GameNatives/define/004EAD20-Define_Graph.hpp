#pragma once
#include <stdint.h>
// Native: DefineGraph; retail VA 0x004EAD20; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004EAD20.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: DefineGraph; retail VA 0x004EAD20; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004EAD20.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 004EAD20 DefineGraph | gate PASS | openigi True */
int DefineGraph(long double context, long double arg2, long double arg3, long double arg4, long double arg5, long double arg6, int arg7, void * arg8, float arg9);
