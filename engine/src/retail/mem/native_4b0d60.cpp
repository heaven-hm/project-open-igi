// Native: Mem_FlagStackPop_4B0D60 (placeholder until VERIFIED); retail VA 0x004B0D60-0x004B0D82 (34 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full listing (a1e060a900 8a0dfd229400 8a542404 48 a3e060a900 8815fd229400 8888e8229400 c3). Caller 0x004BAAC0 passes 1 via push. No callees.
// Behavior/CFG: reads the current index at 0x00A960E0 and the live flag at 0x009422FD, decrements the index, installs the caller's byte as the live flag, then stores the previous flag into the 16-entry saved-flag array at 0x009422E8 indexed by the new index; returns.
// Status/limits: TRANSCRIBED (static only); store order (index, flag, array slot) follows retail instruction order and the array write is not hoisted; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

void Mem_FlagStackPop_4B0D60(std::uint8_t const flag)
{
    std::int32_t index = igi::retail::mem::g_a960e0;          // 0x004B0D60: mov eax,[0xA960E0]
    std::uint8_t const previous = igi::retail::mem::g_9422fd; // 0x004B0D65: mov cl,byte [0x9422FD]
    index -= 1;                                              // 0x004B0D6F: dec eax
    igi::retail::mem::g_a960e0 = index;                      // 0x004B0D70: mov [0xA960E0],eax
    igi::retail::mem::g_9422fd = flag;                       // 0x004B0D75: mov byte [0x9422FD],dl
    igi::retail::mem::g_9422e8[index] = previous;            // 0x004B0D7B: mov byte [eax+0x9422E8],cl
}

} // extern "C"
