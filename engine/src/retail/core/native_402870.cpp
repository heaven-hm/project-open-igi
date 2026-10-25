// Native: Native_00402870; retail VA 0x00402870; original symbol unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 instruction reads; fresh BN/r2/angr capture pending under build/native-analysis/.
// Behavior/CFG: 9 bytes; loads global 0x00567C8C, reads DWORD +0x3C, returns EAX.
// Status/limits: TRANSCRIBED; no paired typed runtime acceptance; no inferred original names.
#include <cstdint>
#include "igi/retail/core/frame_natives.hpp"
#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif
extern "C" std::uint32_t NATIVE_DECL Native_00402870()
{
    return Game_FrameDispatchRecord->divisor;
}

#undef NATIVE_DECL
