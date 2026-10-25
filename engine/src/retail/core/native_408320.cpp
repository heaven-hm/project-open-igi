#include "igi/retail/core/sys_global8_getters.hpp"


#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Native: Sys_GetGlobal8_408320; retail VA 0x00408320.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 on retail IGI.EXE; VA 0x00408320..0x004227877 (6 bytes, 1 block, 0 edges), bytes a0 8b 85 53 00 c3. Reads byte 0 of the run at 0x00538518..0x0053851A.
// Behavior/CFG: `mov al,byte [0x0000538518:8b]; ret` -- one block, unconditional load, no branch and no side effect. Returns the byte in AL; the upper 24 bits of EAX are not cleared, which is what retail leaves.
// Status/limits: STRUCTURALLY_VERIFIED; literal one-block copy. The storage is defined once, in native_408320.cpp, and this file binds to it. No runtime observation in either binary.
extern "C" std::uint8_t NATIVE_DECL Sys_GetGlobal8_408320()
{
    return g_538518;
}
