// Native: Task_GetInteriorPointers_449270 (behavior-derived placeholder: it writes two interior addresses of its first argument and one of that argument's dwords into a three-dword out buffer; retail symbol unknown); retail VA 0x00449270-0x0044928C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 29 bytes (0x1D), 1 block, 0 edges,
//   9 instructions, counted over the exact image. The body ends at the `ret` at 0x0044928C; three 0x90 pad bytes follow and the next
//   function starts at 0x00449290.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x4(%esp),%eax` (0x00449270), `mov 0x8(%esp),%ecx` (0x00449274),
//   `lea 0x20(%eax),%edx` (0x00449278), `mov %edx,(%ecx)` (0x0044927B), `lea 0x70(%eax),%edx` (0x0044927D), `mov %edx,0x4(%ecx)`
//   (0x00449280), `mov 0x178(%eax),%eax` (0x00449283), `mov %eax,0x8(%ecx)` (0x00449289), `ret` (0x0044928C). No branch, no call.
// Behavior/CFG: no loop, no call, no return value. With argument 0 as the base and argument 1 as a three-dword out buffer it performs
//   out[0] = base+0x20, out[1] = base+0x70, out[2] = the dword at base+0x178. The three stores happen in that order and every value is read
//   before it is stored, so no aliasing question arises.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 9-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) The third value's type is not
//   established by this body: it is loaded as a 32-bit dword from base+0x178 and stored as one, so it is carried as std::uint32_t - it may
//   equally be a pointer, and the port makes no claim either way. (c) Nothing here establishes what the regions at base+0x20 and base+0x70
//   are; they are only addresses this routine hands out.
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

void NATIVE_DECL Task_GetInteriorPointers_449270(void* const base, void* const out)
{
    NATIVE_LOG_ENTER("Task_GetInteriorPointers_449270", "0x00449270");
    char* const object = static_cast<char*>(base);
    std::uint32_t* const fields = static_cast<std::uint32_t*>(out);

    // 0x00449270-0x0044927B: out[0] = base+0x20.
    fields[0] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object + 0x20));

    // 0x0044927D-0x00449280: out[1] = base+0x70.
    fields[1] = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object + 0x70));

    // 0x00449283-0x00449289: out[2] = the dword at base+0x178.
    fields[2] = *reinterpret_cast<std::uint32_t*>(object + 0x178);
}

} // extern "C"

#undef NATIVE_DECL
