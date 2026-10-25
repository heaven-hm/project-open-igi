// Native: Mem_DeAllocFn (placeholder until VERIFIED); retail VA 0x004B0D10-0x004B0D39 (42 bytes, 1 block, 2 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 8b1500239400 83c0fc 4a 24fc 56 891500239400 8b30 56 e831ecffff 56 e81b5dffff 83c408 5e c3); calls landed 0x4AF960 and 0x4A6A50.
// Behavior/CFG: deallocation entry used by the task-list destructor. It decrements the live-block count 0x00942300, masks pointer-4 down to a 4-byte boundary, loads the block pointer stored there, removes it from the heap list with 0x4AF960 and releases it through 0x4A6A50.
// Status/limits: BUILD_VERIFIED (static only); the count store happens before the block pointer is loaded, matching retail's instruction order; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

using igi::retail::mem::g_942300;
extern "C" {

// 0x004AF960: the list unlink half, transcribed in engine/src/retail/sys/native_4af960.cpp; C linkage since 2026-10-25. The retail body
// writes only memory, so the returned pointer is the argument itself.
void* Sys_UnlinkListNode_4AF960(void* node);

void Mem_FreeBlock(void* pointer);                  // 0x004B0D30: call 0x4a6a50

void Mem_DeAllocFn(void* const pointer)
{
    const std::uintptr_t masked = (reinterpret_cast<std::uintptr_t>(pointer) - 4U) & ~static_cast<std::uintptr_t>(3U); // 0x004B0D10/0x004B0D1A/0x004B0D1E: add eax,-4; and al,0xfc
    --g_942300;                                                                              // 0x004B0D14/0x004B0D1D/0x004B0D21: mov edx,[0x942300]; dec edx; mov [0x942300],edx
    void** const node = reinterpret_cast<void**>(*reinterpret_cast<std::uint32_t*>(masked)); // 0x004B0D27: mov esi,[eax]
    Sys_UnlinkListNode_4AF960(static_cast<void*>(node));                                     // 0x004B0D2A
    Mem_FreeBlock(node);                                                              // 0x004B0D30
}

} // extern "C"
