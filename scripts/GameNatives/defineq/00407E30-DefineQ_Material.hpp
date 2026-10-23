#pragma once
#include <stdint.h>
// Native: DefineQMaterial; retail VA 0x00407E30; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00407E30.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: DefineQMaterial; retail VA 0x00407E30; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00407E30.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 00407E30 DefineQMaterial | gate PASS | openigi False */
int DefineQMaterial(long double context, long double arg2, long double arg3, int arg4, int arg5);
