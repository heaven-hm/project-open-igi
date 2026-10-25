// Native: Sys_GetGlobal32_448560; retail VA 0x00448560 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00448560-0x00448566 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 fc c1 57 00 c3).
// Behavior/CFG: loads the four-byte value at 0x0057C1FC into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Four bytes is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057C1FC: the four-byte object this getter reads; nothing else in the tree owns it yet.
std::uint32_t g_57c1fc;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_448560(void)
{
    return ::g_57c1fc;                                                       // 0x00448560: mov 0x0057c1fc,%eax; 0x00448565: ret
}

} // extern "C"

#undef NATIVE_DECL
