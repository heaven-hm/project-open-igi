// Native: Task_GetClassId_449600 (behavior-derived placeholder: it writes the 32-bit global at 0x0057C21C into its second argument; retail symbol unknown); retail VA 0x00449600-0x0044960C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 13 bytes (0xD), 1 block, 0 edges,
//   4 instructions, counted over the exact image. The body ends at the `ret` at 0x0044960C; the next function starts at 0x00449610.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x8(%esp),%eax` (0x00449600 - the SECOND stack argument),
//   `mov 0x57c21c,%ecx` (0x00449604), `mov %ecx,(%eax)` (0x0044960A), `ret` (0x0044960C). No branch, no call, no other data reference.
// Behavior/CFG: no loop, no call, no return value. It reads the 32-bit global 0x0057C21C and stores it through the pointer passed as the
//   second argument. The first argument is never read by this body, which is why it is carried as an unnamed parameter rather than dropped:
//   the caller supplies a dword stack slot and this routine simply does not use it.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 4-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) 0x0057C21C is declared here
//   but not defined: its writer is an untranscribed class registrar, so this file reports it undefined until that registrar lands. (c) The
//   first argument's type is not established, because nothing in this body reads it.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057C21C: the class id this routine hands out; its writer is an untranscribed class registrar.
extern std::uint32_t g_57c21c;

void NATIVE_DECL Task_GetClassId_449600(void* const unused_context, void* const out)
{
    (void)unused_context; // 0x00449600 loads only the second argument; the first is never read by this body

    // 0x00449604: mov 0x57c21c,%ecx; 0x0044960A: mov %ecx,(%eax); 0x0044960C: ret.
    *static_cast<std::uint32_t*>(out) = g_57c21c;
}

} // extern "C"

#undef NATIVE_DECL
