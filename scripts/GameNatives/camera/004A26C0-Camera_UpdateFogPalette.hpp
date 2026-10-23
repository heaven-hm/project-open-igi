#pragma once
#include <stdint.h>
// Native: Camera_UpdateFogPalette; retail VA 0x004A26C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE.
// Behavior/CFG: the retail implementation of this entry.
// Status/limits: STRUCTURALLY_VERIFIED.
void Camera_UpdateFogPalette(void *context, float *arg2);
