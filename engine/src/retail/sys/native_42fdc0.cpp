// Native: Sys_GetGlobal16_42FDC0; retail VA 0x0042FDC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0042FDC0-0x0042FDC7 (7 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (66 a1 a8 bd 53 00 c3).
// Behavior/CFG: loads the two-byte value at 0x0053BDA8 into ax and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Two-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053BDA8: the two-byte object this getter reads; nothing else in the tree owns it yet.
std::uint16_t g_53bda8;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_42FDC0(void)
{
    return ::g_53bda8;                                                       // 0x0042FDC0: mov 0x0053bda8,%ax; 0x0042FDC6: ret
}

} // extern "C"

#undef NATIVE_DECL
