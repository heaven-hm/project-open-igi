// Native: Pool_FindBlock_4AAD90 (placeholder until VERIFIED); retail VA 0x004AAD90-0x004AADBA (43 bytes, 1 block, 2 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (a1f421c300 8d0c80 a1f821c300 8d0c88 3bc1 7314 8b542404 2b500c 81fa00001000 7207 83c014 ebe8 33c0 c3); storage owned by 0x4AAD48.
// Behavior/CFG: walks the 0x14-byte pool entries between 0x00C321F8 and 0x00C321F8 + 0x14 * 0x00C321F4 and returns the first entry whose dword at +0x0C is within 0x100000 BELOW the argument (unsigned compare of argument - entry[3]); returns 0 when the walk ends.
// Status/limits: TRANSCRIBED (static only); the unsigned range test (jb) and the 0x14 stride are preserved exactly; storage is reached through the memory subsystem globals owned by 0x4AAD48; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

// The pool globals live in the memory subsystem accessors (MemGlobals) that the
// verified 0x4B0C60 allocation native already reads.
using igi::retail::mem::g_c321f8;
using igi::retail::mem::g_c321f4;

extern "C" {

void* Pool_FindBlock_4AAD90(void* const pointer)
{
    std::uint8_t* entry = static_cast<std::uint8_t*>(g_c321f8);                                    // 0x004AAD98: mov eax, [0xc321f8]
    std::uint8_t* const end = entry + static_cast<std::uintptr_t>(g_c321f4) * 0x14U;               // 0x004AAD95/0x004AAD9D: lea ecx,[eax+eax*4]; lea ecx,[eax+ecx*4]
    while (entry < end)                                                                            // 0x004AADA0: cmp eax, ecx; 0x004AADA2: jae
    {
        const std::uint32_t distance = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pointer) -
                                                                  static_cast<std::uintptr_t>(*reinterpret_cast<std::uint32_t*>(entry + 0x0C)));
        if (distance < 0x100000U)                                                                  // 0x004AADAB: cmp edx,0x100000; 0x004AADB1: jb
        {
            return entry;                                                                          // 0x004AADBA: ret with eax = entry
        }
        entry += 0x14;                                                                             // 0x004AADB3: add eax,0x14
    }
    return nullptr;                                                                                // 0x004AADB8: xor eax,eax
}

} // extern "C"
