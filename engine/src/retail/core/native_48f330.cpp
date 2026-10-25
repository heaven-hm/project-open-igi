#include "igi/retail/core/native_48f330.hpp"


#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Native: SetForcePrecompiled; retail VA 0x0048F330.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 on retail IGI.EXE; VA 0x0048F330..0x0048F339 (9 bytes, 1 block, 0 edges), bytes 8b 44 24 04 a3 f0 8b 5c 00 c3. The one caller is 0x0040100C inside 0x00401000, which is the only site in the image that reaches it.
// Behavior/CFG: `mov eax,dword [esp+4]; mov dword [0x005C8BF0],eax; ret` -- one block, unconditional store, no branch. Returns the argument bits in EAX because the store does not disturb EAX. No error path and no other side effect.
// Status/limits: STRUCTURALLY_VERIFIED; transcription is a literal one-block copy and the caller set is fully enumerated (one site). No candidate runtime observation; the retail store is not observed at runtime, and the flag's consumers are read through Script_GetForcePrecompiledState in engine/src/retail/script/qvm_native_loader.cpp, which now binds to this owner.

extern "C"
{
    std::int32_t g_5c8bf0 = 0;
}

extern "C" std::int32_t NATIVE_DECL SetForcePrecompiled(std::int32_t enabled)
{
    g_5c8bf0 = enabled;
    return enabled;
}
