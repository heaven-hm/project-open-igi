// Native: Task_GetInteriorPointersZeroed_4497B0 (behavior-derived placeholder: like 0x00449270 it hands out two interior addresses of its first argument, but stores a literal zero as the third value; retail symbol unknown); retail VA 0x004497B0-0x004497CA.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 27 bytes (0x1B), 1 block, 0 edges,
//   8 instructions, counted over the exact image. The body ends at the `ret` at 0x004497CA; five 0x90 pad bytes follow and the next
//   function starts at 0x004497D0.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x4(%esp),%ecx` (0x004497B0), `mov 0x8(%esp),%eax` (0x004497B4),
//   `lea 0x20(%ecx),%edx` (0x004497B8), `add $0x70,%ecx` (0x004497BB), `mov %edx,(%eax)` (0x004497BE), `mov %ecx,0x4(%eax)`
//   (0x004497C0), `movl $0x0,0x8(%eax)` (0x004497C3), `ret` (0x004497CA). No branch, no call.
// Behavior/CFG: no loop, no call, no return value. With argument 0 as the base and argument 1 as a three-dword out buffer it performs
//   out[0] = base+0x20, out[1] = base+0x70, out[2] = 0, in that order. It is the same shape as 0x00449270 except that the third value is a
//   literal zero instead of the dword at base+0x178, so the two are transcribed separately rather than folded together.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 8-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) Nothing here establishes what
//   the regions at base+0x20 and base+0x70 are; they are only addresses this routine hands out. (c) The literal zero's meaning at out[2] is
//   not established - only that a 32-bit zero is stored there.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL Task_GetInteriorPointersZeroed_4497B0(void* const base, void* const out)
{
    NATIVE_LOG_ENTER("Task_GetInteriorPointersZeroed_4497B0", "0x004497B0");
    char* const object = static_cast<char*>(base);
    std::uint32_t* const fields = static_cast<std::uint32_t*>(out);

    // 0x004497B0-0x004497BE: out[0] = base+0x20.
    fields[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object + 0x20));

    // 0x004497BB-0x004497C0: out[1] = base+0x70.
    fields[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object + 0x70));

    // 0x004497C3: out[2] = 0.
    fields[2] = 0U;
}

} // extern "C"

#undef NATIVE_DECL
