// Native: QTask_PhaseIdGet548177_4CEA20 (placeholder until VERIFIED); retail VA 0x004CEA20-0x004CEA27 (7 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing a077815400 c3 (mov al, byte [0x548177]; ret); the byte is written only at 0x4C5992 inside 0x4C5900, transcribed as its owner.
// Behavior/CFG: returns the state byte 0x548177; single straight-line block, no arguments.
// Status/limits: STRUCTURALLY_VERIFIED (static only); storage is owned by native_4c5900.cpp and referenced here extern; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint8_t g_548177; // 0x00548177, owned by 0x4C5900

std::uint8_t QTask_PhaseIdGet548177_4CEA20()
{
    return g_548177; // 0x004CEA20: mov al, byte [0x548177]
}

} // extern "C"
