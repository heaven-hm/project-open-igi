// Native: Sys_GetGlobal16_505D40; retail VA 0x00505D40 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00505D40-0x00505D47 (7 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (66 a1 bc a7 a7 00 c3).
// Behavior/CFG: loads the two-byte value at 0x00A7A7BC into ax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Two-byte is the width this load proves; the value's meaning is not established.
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

// 0x00A7A7BC: the two-byte object this getter reads; nothing else in the tree owns it yet.
std::uint16_t g_a7a7bc;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_505D40(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobal16_505D40", "0x00505D40");
    return ::g_a7a7bc;                                                       // 0x00505D40: mov 0x00a7a7bc,%ax; 0x00505D46: ret
}

} // extern "C"

#undef NATIVE_DECL
