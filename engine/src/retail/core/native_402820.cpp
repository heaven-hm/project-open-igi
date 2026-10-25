// Native: Native_00402820; retail VA 0x00402820; original symbol unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 instruction reads; fresh BN/r2/angr capture pending under build/native-analysis/.
// Behavior/CFG: 74 bytes; two calls, repeated context loads, stores at +0x30,+0x50,+0x34,+0x38,+0x3C,+0x44, returns final callee EAX.
// Status/limits: BUILD_VERIFIED; no paired typed runtime acceptance; no inferred original names.
#include <cstdint>
#include "igi/retail/core/frame_natives.hpp"
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
extern "C" {
std::uint32_t NATIVE_DECL Native_00490370();
std::uint32_t NATIVE_DECL Native_004E6030(std::uint32_t);
std::uint32_t NATIVE_DECL Native_00402820(std::uint32_t value)
{
    Game_FrameDispatchRecord->base_low = Native_00490370();
    Game_FrameDispatchRecord->base_high = 0;
    Game_FrameDispatchRecord->counter = 0;
    Game_FrameDispatchRecord->phase_counter = 0;
    Game_FrameDispatchRecord->divisor = value;
    Game_FrameDispatchRecord->unknown_0x44 = 1;
    return Native_004E6030(value);
}
}

#undef NATIVE_DECL
