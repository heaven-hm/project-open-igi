// Native: Script_ParameterEventSlotGetE8 (placeholder until VERIFIED); retail VA 0x004F1BB0-0x004F1BB7 (7 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing a0e8e05400 c3 (mov al, byte [0x54E0E8]; ret); the byte is part of the parameter-event state block g_54e0e4[8] at 0x0054E0E4, owned by 0x4F1A60, index 4 from that base.
// Behavior/CFG: returns the parameter-event state byte 0x54E0E8 and nothing else; single straight-line block.
// Status/limits: BUILD_VERIFIED (static only); storage owned by 0x4F1A60 and referenced here extern; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint8_t g_54e0e4[8]; // 0x0054E0E4..0x0054E0EB, owned by 0x4F1A60

std::uint8_t Script_ParameterEventSlotGetE8()
{
    return g_54e0e4[4]; // 0x004F1BB0: mov al, byte [0x54E0E8]
}

} // extern "C"
