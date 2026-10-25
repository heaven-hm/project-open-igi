// Native: QTask_HandlerD_42FEF0; retail VA 0x0042FEF0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0042FEF0-0x0042FEFA (11 bytes, 1 block, 0 edges, 3 instructions).
// Tools/evidence: GNU objdump i386 disassembly build/retail-dis.txt (0x0042FEF0-0x0042FEFA), bytes 8b 44 24 08 c7 00 f0 02 00 00 c3; ABI and the loader call site in docs/fidelity/evidence/qtask-handler-abi.md.
// Behavior/CFG: the size callback of the class whose id word is 0x0057BD30 ("Wheel"), linked at the private phase byte 0x0054DC31 by 0x0042FE40. The loader 0x004EDD20 calls it at 0x004EDF09 with (0, &size_slot), where &size_slot is the address of the loader's own third argument slot; the handler stores the class block size 0x2F0 through that pointer and returns the pointer unchanged in EAX (the load into EAX at 0x0042FEF0 is the only value EAX ever holds). It never reads its first argument and has no branch, no global and no callee. Single block.
// Status/limits: BUILD_VERIFIED (static only; no build and no runtime). 0x0042FE80 writes exactly this 0x2F0-byte block (last field at +0x2EC), so the constant is corroborated by the parse callback; the loader ignores the returned EAX, but the retail returns the pointer and this declaration preserves that. The candidate compiles for the x86 target as a single translation unit (i686-w64-mingw32-g++ 16.2.0, -std=c++20 -O2) to the retail's exact three instructions (mov 0x8(%esp),%eax; movl $0x2f0,(%eax); ret); the repo build target does not list this file yet.
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

std::uint32_t* NATIVE_DECL QTask_HandlerD_42FEF0(std::uint32_t arg1, std::uint32_t* size_out)
{
    NATIVE_LOG_ENTER("QTask_HandlerD_42FEF0", "0x0042FEF0");
    (void)arg1;                                     // 0x0042FEF0-0x0042FEFA: the first argument is never read
    *size_out = 0x2F0U;                             // 0x0042FEF4: movl $0x2f0,(%eax)
    return size_out;                                // 0x0042FEFA: ret with EAX still holding the second argument
}                                                   // 0x0042FEFB-0x0042FEFF: nop padding

} // extern "C"

#undef NATIVE_DECL
