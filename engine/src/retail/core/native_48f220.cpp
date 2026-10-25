#include "igi/retail/core/native_48f220.hpp"


#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Native: GetStartupMode; retail VA 0x0048F220.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 on retail IGI.EXE; VA 0x0048F220..0x0048F225 (6 bytes, 1 block, 0 edges), bytes a1 08 8c 5c 00 c3. The one caller is 0x00401020 inside 0x00401000, which is the only site in the image that reaches it.
// Behavior/CFG: `mov eax,dword [0x005C8C08]; ret` -- one block, unconditional load, no branch and no side effect. Returns the flag word itself, not a narrowed or normalized form of it.
// Status/limits: STRUCTURALLY_VERIFIED; literal one-block copy and the caller set is fully enumerated (one site). The storage it reads is owned by the writer 0x0048F210 (Game_SetStartupMode, engine/src/retail/core/app_startup_globals.cpp); this file only binds to it. No candidate runtime observation; retail runtime for the writer is recorded as two calls with args 1 then 0, but the read side has no paired capture.

extern "C" std::int32_t NATIVE_DECL GetStartupMode()
{
    return AppContext_StartupMode;
}
