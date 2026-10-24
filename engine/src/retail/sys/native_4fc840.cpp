// Native: Sys_GetGlobal8_4FC840; retail VA 0x004FC840 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FC840-0x004FC846 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 cc e4 54 00 c3).
// Behavior/CFG: loads the one-byte value at 0x0054E4CC into al and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. One-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054E4CC: the one-byte object this getter reads; nothing else in the tree owns it yet.
std::uint8_t g_54e4cc;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_4FC840(void)
{
    return ::g_54e4cc;                                                       // 0x004FC840: mov 0x0054e4cc,%al; 0x004FC845: ret
}

} // extern "C"

#undef NATIVE_DECL
