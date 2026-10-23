#pragma once
#include <stdint.h>
#include <stdbool.h>
// Native: WeaponGun_ProcessWNES; retail VA 0x00471120; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00471120.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.
// Native: WeaponGun_ProcessWNES; retail VA 0x00471120; signature from the retail analysis of igi.exe.
// Artifact: candidate transcription of the retail function at 0x00471120.
// Tools/evidence: Binary Ninja and Radare2 CFG captured from the retail igi.exe; address confirmed by Binary Ninja, Radare2 and angr.
// Behavior/CFG: CFG agreement between Binary Ninja and Radare2 for this entry; the candidate body is the transcription under review.
// Status/limits: STRUCTURALLY_VERIFIED; retail behaviour is compared by the configured differential runner, not by this comment.

/* 00471120 WeaponGun_ProcessWNES | gate PASS | openigi True */
int WeaponGun_ProcessWNES("regparm") WeaponGun_ProcessWNES(int arg1, int arg2, int arg3, void* arg4);
