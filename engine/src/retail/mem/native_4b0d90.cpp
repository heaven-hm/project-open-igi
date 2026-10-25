// Native: Mem_FlagStackPush (placeholder until VERIFIED); retail VA 0x004B0D90-0x004B0DA8 (24 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full listing (a1e060a900 8a88e8229400 40 880dfd229400 a3e060a900 c3). No callees.
// Behavior/CFG: reads the current index at 0x00A960E0, loads the saved flag from the array at 0x009422E8 at that index, increments the index, installs the saved byte as the live flag and stores the incremented index back; returns.
// Status/limits: STRUCTURALLY_VERIFIED (static only); load order and the increment-then-store sequence follow retail; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

void Mem_FlagStackPush()
{
    std::int32_t index = igi::retail::mem::g_a960e0;                 // 0x004B0D90: mov eax,[0xA960E0]
    std::uint8_t const saved = igi::retail::mem::g_9422e8[index];     // 0x004B0D95: mov cl,byte [eax+0x9422E8]
    index += 1;                                                      // 0x004B0D9B: inc eax
    igi::retail::mem::g_9422fd = saved;                              // 0x004B0D9C: mov byte [0x9422FD],cl
    igi::retail::mem::g_a960e0 = index;                              // 0x004B0DA2: mov [0xA960E0],eax
}

} // extern "C"
