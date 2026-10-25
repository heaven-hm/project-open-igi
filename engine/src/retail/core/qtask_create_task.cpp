#include "igi/retail/core/app_context_extra.hpp"
#include "igi/retail/core/app_startup_natives.hpp"
#include "igi/retail/core/class_registration.hpp"
#include "igi/retail/core/flow.hpp"
#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/native_48f220.hpp"
#include "igi/retail/core/native_48f330.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include <cstdint>

// Port-owned copy of the 32-bit startup-mode flag at 0x005C8C08. Retail main
// sets it to 1 at 0x0048F656 (call to 0x0048F210) before it calls 0x00401000
// at 0x0048F7AA. The writer 0x0048F210 is transcribed as Game_SetStartupMode in
// engine/src/retail/core/app_startup_globals.cpp and writes this same storage;
// the initialiser here only fixes the pre-startup value, since a static
// initialiser cannot be a call. It has external linkage so the reader at
// 0x0048F220 (engine/src/retail/core/native_48f220.cpp) and the writer both bind
// to this one definition, and so the compiler keeps the retail branch instead of
// constant-folding it.
extern "C"
{
    std::int32_t AppContext_StartupMode = 1;
}

// Native: QTask_CreateTask; retail VA 0x00401000.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 and angr CFGFast on retail IGI.EXE; VA 0x00401000..0x00401042 (67 bytes, 3 blocks, 2 direct edges), bytes 68 44 61 53 00 e8 f6 e0 08 00 6a 01 e8 1f e3 08 00 e8 1a 19 00 00 6a 00 e8 e3 17 00 00 83 c4 0c e8 fb e1 08 00 85 c0 75 0d 6a 04 e8 70 18 00 00 83 c4 04 b0 01 c3 6a 02 e8 63 18 00 00 83 c4 04 b0 01 c3.
// Behavior/CFG: calls 0x0048F100 with the "IGI" string at 0x00536144, calls 0x0048F330(1) to set the force-precompiled flag at 0x005C8BF0, calls QTask_RegisterClasses at 0x00402930, calls Flow_CreateHandler at 0x00402800 with null, reads the startup-mode flag at 0x005C8C08 through 0x0048F220, and calls Flow_SetState at 0x004028A0 with 2 when it is non-zero or 4 when it is zero; both paths then set AL to 1 and return, so the observed int32 result is 1.
// Status/limits: STRUCTURALLY_VERIFIED; all three of this body's retail callees are now their own transcribed natives rather than translation-unit-local stand-ins -- SetAppTitle 0x0048F100 (core/app_context_extra.cpp), SetForcePrecompiled 0x0048F330 (core/native_48f330.cpp) and GetStartupMode 0x0048F220 (core/native_48f220.cpp) -- so the call sequence is the retail sequence. Limits: QTask_RegisterClasses 0x00402930 still reports its untranscribed registrars and throws before the flow calls, the candidate main does not yet call 0x00401000, and no paired retail/candidate runtime observation exists.
extern "C" int NATIVE_DECL QTask_CreateTask()
{
    SetAppTitle("IGI");
    SetForcePrecompiled(1);
    QTask_RegisterClasses();
    Flow_CreateHandler(nullptr);

    // Retail 0x00401025..0x00401042: test the startup flag, call Flow_SetState
    // with 2 (non-zero) or 4 (zero), then `mov al, 1; retn`. The low byte of
    // Flow_SetState's EAX result is replaced, exactly as the retail does.
    const std::uint32_t state_result = (GetStartupMode() != 0) ? Flow_SetState(2U) : Flow_SetState(4U);
    return static_cast<std::int32_t>((state_result & 0xFFFFFF00U) | 1U);
}

#undef NATIVE_DECL
