#pragma once
#include <stdint.h>
#include <stdbool.h>
// Native: WeatherWaterSky_Invoke7_5236C0; retail VA 0x005236C0; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x005236C0.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: WeatherWaterSky_Invoke7_5236C0; retail VA 0x005236C0; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x005236C0.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.

/* 005236C0 WeatherWaterSky_Invoke7_5236C0 | gate PASS | openigi True */
int WeatherWaterSky_Invoke7_5236C0("regparm") WeatherWaterSky_Invoke7_5236C0(int arg1, int arg2, int* arg3, void* arg4);
