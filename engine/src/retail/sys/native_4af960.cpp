// Native: Sys_UnlinkListNode_4AF960; retail VA 0x004AF960 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AF960-0x004AF980 (33 bytes, 1 block, 0 edges, 10 instructions; the extent is measured from the image - the next byte 0x004AF981 is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image; no call, no global, one argument. The two-store pairs at 0x004AF969/0x004AF970 are the two neighbours' cross-links, and the pair at 0x004AF973/0x004AF97A is the unlinked node's own links cleared.
// Behavior/CFG: unlinks one node from the doubly-linked list whose node layout is +0x00 forward and +0x04 back (the layout 0x004AF8F0 and its initialiser 0x004F1D60 jointly establish). The node's back neighbour at +0x04 gets its forward pointer from the node's +0x00; the node's forward neighbour at +0x00 gets its back pointer from the node's +0x04; then both of the node's own pointers are zeroed. Straight-line, no branch, no call, no global; the head is not touched, and there is no list count here.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. This is the remove half of the list pair whose insert half is 0x004AF8F0. The legacy `igi::retail::mem::HeapList_Remove` copy in mem/heap_list.cpp was deleted on 2026-10-25, so this file is the VA's only implementation. C linkage at file scope since 2026-10-25: the wrapper was `namespace igi::retail::sys`, which made the exported symbol `__ZN3igi6retail3sys25Sys_UnlinkListNode_4AF960EPv` while one caller (core/native_4f9df0.cpp) already referenced the unqualified name, so all seven files were moved to C linkage together and definition and callers now agree. Return: the body only writes ECX/EDX and memory, so EAX on return is still the argument, and an existing transcribed caller (core/native_48f4f0.cpp) uses that value as the remove result; the declaration says `void*` and returns the node so that observable survives without inventing an instruction. No field beyond the node's first two pointers is established by this body, so the node stays a byte pointer. The sentinel/head geometry is only what 0x004AF8F0 and 0x004F1D60 establish.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* NATIVE_DECL Sys_UnlinkListNode_4AF960(void* node)
{
    std::uint8_t* const item = static_cast<std::uint8_t*>(node);                       // 0x4AF960: mov 0x4(%esp),%eax
    std::uint8_t* back = *reinterpret_cast<std::uint8_t**>(item + 0x04U);               // 0x4AF964: mov 0x4(%eax),%ecx
    std::uint8_t* forward = *reinterpret_cast<std::uint8_t**>(item);                    // 0x4AF967: mov (%eax),%edx
    *reinterpret_cast<std::uint8_t**>(back) = forward;                                  // 0x4AF969: mov %edx,(%ecx)
    back = *reinterpret_cast<std::uint8_t**>(item);                                     // 0x4AF96B: mov (%eax),%ecx
    forward = *reinterpret_cast<std::uint8_t**>(item + 0x04U);                          // 0x4AF96D: mov 0x4(%eax),%edx
    *reinterpret_cast<std::uint8_t**>(back + 0x04U) = forward;                          // 0x4AF970: mov %edx,0x4(%ecx)
    *reinterpret_cast<std::uint8_t**>(item + 0x04U) = nullptr;                          // 0x4AF973: movl $0x0,0x4(%eax)
    *reinterpret_cast<std::uint8_t**>(item) = nullptr;                                  // 0x4AF97A: movl $0x0,(%eax)
    return item;                                                                        // 0x4AF980: ret (EAX still holds the argument, untouched by the body)
}

} // extern "C"

#undef NATIVE_DECL
