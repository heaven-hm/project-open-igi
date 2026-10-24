// Native: Sys_GetGlobal8_4F1200; retail VA 0x004F1200 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1200-0x004F1206 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a0 a5 dd 54 00 c3).
// Behavior/CFG: loads the one-byte value at 0x0054DDA5 into al and returns it; no argument, no branch, no store, no call.
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

// 0x0054DDA5: one-byte object, defined by native_4ee970_globals.cpp which assigns it; this file only reads it.
extern std::uint8_t g_54dda5;

std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F1200(void)
{
    return ::g_54dda5;                                                       // 0x004F1200: mov 0x0054dda5,%al; 0x004F1205: ret
}

} // extern "C"

#undef NATIVE_DECL
