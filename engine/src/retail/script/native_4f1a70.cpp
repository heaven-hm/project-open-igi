// Native: Script_PhaseIdGetE5_4F1A70 (placeholder until VERIFIED); retail VA 0x004F1A70-0x004F1A73 (4 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a0 e5 e0 54 00 c3; xref writers 0x004F1840 (0x004F186B/0x4F1878) and 0x004F18C0 (0x004F1942/0x4F1947).
// Behavior/CFG: mov al,[0x0054E0E5]; ret - returns the second script byte in AL.
// Status/limits: STRUCTURALLY_VERIFIED; image byte 0xFF (file offset 0x14E0E5); runtime-written, only the untranscribed writers change it. ABI note as 0x4F1A60. Runtime UNOBSERVED.
#include "igi/retail/script/script_state.hpp"

extern "C" std::uint8_t g_54e0e5 = 0xFFU; // retail 0x0054E0E5, image value 0xFF

extern "C" {

std::uint8_t Script_PhaseIdGetE5_4F1A70()
{
    return g_54e0e5; // 0x004F1A70: a0 e5 e0 54 00 (mov al,[0x54E0E5]); c3
}

} // extern "C"
