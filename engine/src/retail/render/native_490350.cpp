// Native: Display_SetSwapFlag (placeholder until VERIFIED); retail VA 0x00490350-0x00490359 (10 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes 8a 44 24 04 a2 60 8c c2 00 c3; DINPUT-LAYER packet §6.
// Behavior/CFG: al = byte [esp+4]; mov [0x00C28C60],al; ret - stores the caller's byte.
// Status/limits: TRANSCRIBED; byte is .data zero-fill runtime data. Runtime UNOBSERVED.
#include "igi/retail/render/display_state_flags.hpp"

extern "C" std::uint8_t g_c28c60 = 0U; // 0x00C28C60, image init 0

extern "C" {

void Display_SetSwapFlag(char flag)
{
    g_c28c60 = static_cast<std::uint8_t>(flag); // 0x00490350: 8a 44 24 04; a2 60 8c c2 00
}

} // extern "C"
