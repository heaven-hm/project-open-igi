// Native: Sys_GetGlobal32_4D0D80; retail VA 0x004D0D80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004D0D80-0x004D0D86 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 6c 83 54 00 c3).
// Behavior/CFG: loads the four-byte value at 0x0054836C into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Four bytes is the width this load proves; the value's meaning is not established.
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

// 0x0054836C: the four-byte object this getter reads; nothing else in the tree owns it yet.
std::uint32_t g_54836c= 0xFFU;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4D0D80(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobal32_4D0D80", "0x004D0D80");
    return ::g_54836c;                                                       // 0x004D0D80: mov 0x0054836c,%eax; 0x004D0D85: ret
}

} // extern "C"

#undef NATIVE_DECL
