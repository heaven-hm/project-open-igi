// Native: Display_SetVSyncFlag_490340 (placeholder until VERIFIED); retail VA 0x00490340-0x00490346 (7 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes c6 05 61 8c c2 00 01 c3; DINPUT-LAYER packet §6.
// Behavior/CFG: mov byte [0x00C28C61],1; ret - sets the vsync/cooperative-level flag byte.
// Status/limits: TRANSCRIBED; the byte is .data zero-fill, runtime data (no callees). Runtime UNOBSERVED.
#include "igi/retail/render/display_state_flags.hpp"

extern "C" std::uint8_t g_c28c61 = 0U; // 0x00C28C61, image init 0

extern "C" {

void Display_SetVSyncFlag_490340()
{
    g_c28c61 = 1U; // 0x00490340: c6 05 61 8c c2 00 01
}

} // extern "C"
