#pragma once
#include <stdint.h>
// Native: Input_GetDIErrStr; retail VA 0x0048F970; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x0048F970.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: Input_GetDIErrStr; retail VA 0x0048F970; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x0048F970.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
/* 0048F970 Input_GetDIErrStr | gate PASS | openigi True */
LRESULT Input_GetDIErrStr(HWND context, unsigned int arg2, WPARAM arg3, LPARAM arg4);
