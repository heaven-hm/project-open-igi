// Native: QTask_PhaseIdGet548176_4CEA10 (placeholder until VERIFIED); retail VA 0x004CEA10-0x004CEA17 (7 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing a076815400 c3 (mov al, byte [0x548176]; ret); the byte is written only at 0x4C5983 inside 0x4C5900, transcribed as its owner.
// Behavior/CFG: returns the state byte 0x548176; single straight-line block, no arguments.
// Status/limits: TRANSCRIBED (static only); storage is owned by native_4c5900.cpp and referenced here extern; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint8_t g_548176; // 0x00548176, owned by 0x4C5900

std::uint8_t QTask_PhaseIdGet548176_4CEA10()
{
    return g_548176; // 0x004CEA10: mov al, byte [0x548176]
}

} // extern "C"
