// Native: Sys_GetGlobal8_4F1FE0; retail VA 0x004F1FE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1FE0-0x004F1FE6 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 09 6c a7 00 c3).
// Behavior/CFG: loads the one-byte value at 0x00A76C09 into al and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. One-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A76C09: one-byte object, defined by native_4f1ee0.cpp which assigns it; this file only reads it.
extern std::uint8_t g_a76c09;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F1FE0(void)
{
    return ::g_a76c09;                                                       // 0x004F1FE0: mov 0x00a76c09,%al; 0x004F1FE5: ret
}

} // extern "C"

#undef NATIVE_DECL
