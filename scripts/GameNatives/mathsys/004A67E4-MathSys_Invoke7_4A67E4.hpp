#pragma once
#include <stdint.h>
#include <stdbool.h>
// Native: MathSys_Invoke7_4A67E4; retail VA 0x004A67E4; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004A67E4.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: MathSys_Invoke7_4A67E4; retail VA 0x004A67E4; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004A67E4.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.

/* 004A67E4 MathSys_Invoke7_4A67E4 | gate PASS | openigi True */
int MathSys_Invoke7_4A67E4("regparm") MathSys_Invoke7_4A67E4(char* arg1, short arg2, void* arg3, void* arg4, int arg5);
