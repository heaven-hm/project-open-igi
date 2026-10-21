#pragma once
#include <stdint.h>
// Native: Mem_AllocFn; retail VA 0x004B0C60.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE.
// Behavior/CFG: the retail implementation of this entry.
// Status/limits: STRUCTURALLY_VERIFIED.
void *Mem_AllocFn(int context, int arg2);
