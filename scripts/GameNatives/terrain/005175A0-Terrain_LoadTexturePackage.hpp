#pragma once
#include <stdint.h>
// Native: Terrain_LoadTexturePackage; retail VA 0x005175A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE.
// Behavior/CFG: the retail loads the terrain texture package through the resource system.
// Status/limits: STRUCTURALLY_VERIFIED.
void Terrain_LoadTexturePackage(void *context, const char *arg2);
