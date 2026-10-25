// Native: Script_ParameterEventInit (placeholder until VERIFIED); retail VA 0x004F1840-0x004F18BA (123 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing saved at docs/fidelity/evidence/0x004F1840-listing.txt (33c0 57 a3c858a7.. b9c0040000 a3cc58a7.. bfe058a7.. a3d058a7.. f3ab 50 a3d458a7.. e857fff0ff 6a00 a2e4e054.. e84bfff0ff 83c408 a2e5e054.. e89e020000 a2ebe054.. e894020000 a2eae054.. e88a020000 a2e7e054.. e880020000 a2e8e054.. e876020000 a2e9e054.. e86c020000 a2e6e054.. 5f c3); callees QTask_AllocateEvent (0x4017C0) and 0x4F1B20 both transcribed; 0x54E0E4/0x54E0E5/0x54E0EB-E6 state bytes written here, so this native owns them.
// Behavior/CFG: zeroes the four dword cells at 0xA758C8/CC/D0/D4, clears 0x4C0 dwords at 0xA758E0 with rep stosd, allocates two task events (both with argument 0) storing the byte results at 0x54E0E4/0x54E0E5, then allocates six parameter-event slots through 0x4F1B20 storing them at 0x54E0EB, EA, E7, E8, E9, E6 in exactly that order. Single straight-line block.
// Status/limits: BUILD_VERIFIED (static only); the six slot stores keep retail's non-obvious order; storage is declared here because this routine is its writer; runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

extern std::uint8_t g_a758c8[16]; // 0x00A758C8, owned by 0x4F1B20
std::int32_t g_a758cc = 0;      // 0x00A758CC
std::int32_t g_a758d0 = 0;      // 0x00A758D0
std::int32_t g_a758d4 = 0;      // 0x00A758D4
std::uint32_t g_a758e0[0x4C0] = {}; // 0x00A758E0, 0x4C0 dwords cleared by rep stosd
extern std::uint8_t g_54e0e4[8]; // 0x0054E0E4..0x0054E0EB, owned by 0x4F1A60

std::int32_t Script_ParameterEventSlotAlloc();

void Script_ParameterEventInit()
{
    *reinterpret_cast<std::int32_t*>(g_a758c8) = 0;                       // 0x004F1843
    *reinterpret_cast<std::int32_t*>(&g_a758cc) = 0;                      // 0x004F184D
    *reinterpret_cast<std::int32_t*>(&g_a758d0) = 0;                      // 0x004F1857
    for (std::int32_t i = 0; i < 0x4C0; i += 1)                           // 0x004F185C: rep stosd
        g_a758e0[i] = 0;
    *reinterpret_cast<std::int32_t*>(&g_a758d4) = 0;                      // 0x004F185F
    std::uint8_t const first = QTask_AllocateEvent(0); // 0x004F1864
    g_54e0e4[0] = first;                                                  // 0x004F186B
    std::uint8_t const second = QTask_AllocateEvent(0); // 0x004F1870
    g_54e0e4[1] = second;                                                 // 0x004F1878
    g_54e0e4[7] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F1882 -> 0x54E0EB
    g_54e0e4[6] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F188C -> 0x54E0EA
    g_54e0e4[3] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F1896 -> 0x54E0E7
    g_54e0e4[4] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F18A0 -> 0x54E0E8
    g_54e0e4[5] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F18AA -> 0x54E0E9
    g_54e0e4[2] = static_cast<std::uint8_t>(Script_ParameterEventSlotAlloc()); // 0x004F18B4 -> 0x54E0E6
}

} // extern "C"
