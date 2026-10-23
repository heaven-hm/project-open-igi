#pragma once
#include <stdint.h>
// Native: _Script_SetWarningLevel; retail VA 0x004B8C60; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004B8C60.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: _Script_SetWarningLevel; retail VA 0x004B8C60; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x004B8C60.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 004B8C60 _Script_SetWarningLevel | gate PASS | openigi False */
int _Script_SetWarningLevel(int context, float arg2);
