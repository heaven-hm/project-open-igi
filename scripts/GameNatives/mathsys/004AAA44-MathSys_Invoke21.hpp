#pragma once
#include <stdint.h>
#include <stdbool.h>
// Native: MathSys_Invoke21; retail VA 0x004AAA44; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004AAA44.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: MathSys_Invoke21; retail VA 0x004AAA44; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004AAA44.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.

/* 004AAA44 MathSys_Invoke21 | gate PASS | openigi True */
int MathSys_Invoke21("regparm") MathSys_Invoke21(int arg1, int arg2, int arg3, int* arg4, void* arg5, void* arg6, int arg7);
