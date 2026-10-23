#pragma once
#include <stdint.h>
// Native: Terrain_DecodeChunkCommands; retail VA 0x00510010.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE.
// Behavior/CFG: the retail implementation of this entry.
// Status/limits: STRUCTURALLY_VERIFIED.
int Terrain_DecodeChunkCommands(void *context);
