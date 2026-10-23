#pragma once
#include <stdint.h>
// Native: SetWindowHandle; retail VA 0x0048F0E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0E0..0x0048F0EA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at the window handle global 0x005C8BC4, and returns the same bits in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the store and the EAX return match the retail exactly.
void *SetWindowHandle(void *window);
