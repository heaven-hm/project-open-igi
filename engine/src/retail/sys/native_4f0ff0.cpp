// Native: Sys_GetGlobal32_4F0FF0; retail VA 0x004F0FF0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0FF0-0x004F0FF6 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 6c 18 a7 00 c3).
// Behavior/CFG: loads the four-byte value at 0x00A7186C into eax and returns it; no argument, no branch, no store, no call.
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

// 0x00A7186C: four-byte object. The ownership scan matched a mention of this address in another file's evidence, not a
// definition, so no owner exists and this file defines it. Was previously: hash_table_globals.cpp which assigns it; this file only reads it.
std::uint32_t g_a7186c;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FF0(void)
{
    return ::g_a7186c;                                                       // 0x004F0FF0: mov 0x00a7186c,%eax; 0x004F0FF5: ret
}

} // extern "C"

#undef NATIVE_DECL
