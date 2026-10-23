#pragma once
#include <stdint.h>
// Native: GOGfxDevice; retail VA 0x00404010; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00404010.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: GOGfxDevice; retail VA 0x00404010; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00404010.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 00404010 GOGfxDevice | gate FAIL | openigi False */
int GOGfxDevice(int context, float arg2);
