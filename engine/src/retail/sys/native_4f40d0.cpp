// Native: Sys_GetGlobal8_4F40D0; retail VA 0x004F40D0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F40D0-0x004F40D6 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 da e1 54 00 c3).
// Behavior/CFG: loads the one-byte value at 0x0054E1DA into al and returns it; no argument, no branch, no store, no call.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. One-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054E1DA: the one-byte object this getter reads; nothing else in the tree owns it yet.
std::uint8_t g_54e1da;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F40D0(void)
{
    return ::g_54e1da;                                                       // 0x004F40D0: mov 0x0054e1da,%al; 0x004F40D5: ret
}

} // extern "C"

#undef NATIVE_DECL
