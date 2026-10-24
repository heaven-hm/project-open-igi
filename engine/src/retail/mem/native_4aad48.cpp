// Native: Pool_Init_4AAD48 (placeholder until VERIFIED); retail VA 0x004AAD48-0x004AAD8F (71 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (6840010000 6a00 ff352824c300 ff15ac305300 85c0 a3f821c300 7501 c3 8b4c2404 8325f021c3..00 8325f421c3..00 6a01 a3ec21c300 890dfc21c300 c705e421c3..10000000 58 c3); imports KERNEL32 HeapAlloc directly.
// Behavior/CFG: allocates the 0x140-byte small-block pool with HeapAlloc(g_c32428, 0, 0x140), stores the block at 0x00C321F8 and returns 0 immediately when the allocation fails; otherwise clears 0x00C321F0 and 0x00C321F4, stores the block at 0x00C321EC, stores the argument at 0x00C321FC, stores 0x10 at 0x00C321E4 and returns 1 (retail's trailing push 1 / pop eax).
// Status/limits: TRANSCRIBED (static only); this native OWNS the pool globals 0x00C321E4/EC/F0/F4/F8 and 0x00C321FC, which it reaches through the memory subsystem's MemGlobals accessors already used by the verified 0x4B0C60 allocation native; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

// The pool globals live in the memory subsystem accessors (MemGlobals) that the
// verified 0x4B0C60 allocation native already reads.
using igi::retail::mem::g_c321ec;
using igi::retail::mem::g_c321f0;
using igi::retail::mem::g_c321f4;
using igi::retail::mem::g_c321f8;
using igi::retail::mem::g_c321fc;
using igi::retail::mem::g_c321e4;

extern "C" {


std::int32_t Pool_Init_4AAD48(void* const argument)
{
    void* const block = igi::retail::mem::Mem_HeapAlloc(0x140U); // 0x004AAD48..0x004AAD55: push 0x140; push 0; push [0xc32428]; call HeapAlloc
    g_c321f8 = block;                          // 0x004AAD5D: mov dword [0xc321f8], eax
    if (block == nullptr)                      // 0x004AAD5B: test eax,eax; 0x004AAD62: jne
    {
        return 0;                              // 0x004AAD64: ret with eax still 0
    }
    g_c321f0 = 0U;                             // 0x004AAD69: and dword [0xc321f0], 0
    g_c321f4 = 0U;                             // 0x004AAD70: and dword [0xc321f4], 0
    g_c321ec = block;                          // 0x004AAD79: mov dword [0xc321ec], eax
    g_c321fc = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(argument)); // 0x004AAD7E: mov dword [0xc321fc], ecx
    g_c321e4 = 0x10U;                          // 0x004AAD84: mov dword [0xc321e4], 0x10
    return 1;                                  // 0x004AAD77: push 1; 0x004AAD8E: pop eax
}

} // extern "C"
