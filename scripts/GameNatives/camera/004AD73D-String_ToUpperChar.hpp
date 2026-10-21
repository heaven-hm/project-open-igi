#pragma once
#include <stdint.h>
// Native: String_ToUpperChar; retail VA 0x004AD73D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE.
// Behavior/CFG: the retail implementation of this entry.
// Status/limits: STRUCTURALLY_VERIFIED.
char String_ToUpperChar(char value);
