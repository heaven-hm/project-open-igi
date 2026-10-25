// Native: Script_PhaseIdGetE4_4F1A60 (placeholder until VERIFIED); retail VA 0x004F1A60-0x004F1A63 (4 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a0 e4 e0 54 00 c3; xref writers 0x004F1840 (0x004F186B/0x4F1878) and 0x004F18C0 (0x004F1942/0x4F1947).
// Behavior/CFG: mov al,[0x0054E0E4]; ret - returns the script byte in AL; upper EAX bits are left untouched by retail.
// Status/limits: STRUCTURALLY_VERIFIED; image byte 0xFF (file offset 0x14E0E4); runtime-written, only the untranscribed writers change it. ABI note: the port returns uint8_t (zero-extends) where retail preserves upper EAX bits; callers in scope read AL only. Runtime UNOBSERVED.
#include "igi/retail/script/script_state.hpp"

extern "C" std::uint8_t g_54e0e4 = 0xFFU; // retail 0x0054E0E4, image value 0xFF

extern "C" {

std::uint8_t Script_PhaseIdGetE4_4F1A60()
{
    return g_54e0e4; // 0x004F1A60: a0 e4 e0 54 00 (mov al,[0x54E0E4]); c3
}

} // extern "C"
