// Native: QTask_UnlinkTask_4015D0 (placeholder until VERIFIED); retail VA 0x004015D0-0x004015E2 (19 bytes, 2 blocks, 1 branch).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 linear and CFG decode of the retail bytes; docs/fidelity/evidence/0x4EE970-data-extents.md section 16.
// Behavior/CFG: one cdecl pointer argument; loads the owner word at +0x14 and, only when it is non-null, calls the heap-list removal native 0x004AF960 with that object pointer and pops the argument back off; returns nothing.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED because the startup boundary precedes every caller. Depends on HeapList_Remove 0x004AF960, which the tree already provides; the destructor 0x004015F0 is the caller that matters and is still untranscribed.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER)
#define NATIVE_DECL __cdecl
#elif defined(__GNUC__) && defined(__i386__)
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004AF960: the list unlink half, transcribed in engine/src/retail/sys/native_4af960.cpp; C linkage since 2026-10-25.
void* Sys_UnlinkListNode_4AF960(void* node);

void NATIVE_DECL QTask_UnlinkTask_4015D0(void* task)
{
    NATIVE_LOG_ENTER("QTask_UnlinkTask_4015D0", "0x004015D0");
    if (*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(task) + 0x14) != 0U) // 0x004015D4/0x004015D7
    {
        Sys_UnlinkListNode_4AF960(static_cast<void*>(task));                     // 0x004015DC -> 0x004AF960
    }
}

} // extern "C"

#undef NATIVE_DECL
