// Native: Sys_GetGlobal32_443E90; retail VA 0x00443E90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00443E90-0x00443E96 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 a0 c1 57 00 c3).
// Behavior/CFG: loads the four-byte value at 0x0057C1A0 into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Four bytes is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057C1A0: the four-byte object this getter reads; nothing else in the tree owns it yet.
std::uint32_t g_57c1a0;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_443E90(void)
{
    return ::g_57c1a0;                                                       // 0x00443E90: mov 0x0057c1a0,%eax; 0x00443E95: ret
}

} // extern "C"

#undef NATIVE_DECL
