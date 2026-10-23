#pragma once
#include <stdint.h>
#include <stdbool.h>
// Native: MathSys_Invoke19; retail VA 0x004AA0A6; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004AA0A6.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: MathSys_Invoke19; retail VA 0x004AA0A6; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004AA0A6.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.

/* 004AA0A6 MathSys_Invoke19 | gate PASS | openigi True */
void* MathSys_Invoke19("regparm") MathSys_Invoke19(int arg1, char* arg2);
