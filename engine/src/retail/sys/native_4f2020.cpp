// Native: Sys_GetGlobal32_4F2020; retail VA 0x004F2020 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F2020-0x004F2026 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 fc 6b a7 00 c3).
// Behavior/CFG: loads the four-byte value at 0x00A76BFC into eax and returns it; no argument, no branch, no store, no call.
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

extern std::uint32_t g_a76bfc;

// 0x00A76BFC: the four-byte object this getter reads. It is defined by native_4f1ee0.cpp (the routine that
// assigns it at 0x004F1EE0); this file only reads it, so it declares the object extern rather than defining a second copy.

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F2020(void)
{
    return ::g_a76bfc;                                                       // 0x004F2020: mov 0x00a76bfc,%eax; 0x004F2025: ret
}

} // extern "C"

#undef NATIVE_DECL
