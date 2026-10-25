// Native: Sys_GetGlobal16_4E0830; retail VA 0x004E0830 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004E0830-0x004E0837 (7 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (66 a1 98 86 54 00 c3).
// Behavior/CFG: loads the two-byte value at 0x00548698 into ax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Two-byte is the width this load proves; the value's meaning is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00548698: the two-byte object this getter reads; nothing else in the tree owns it yet.
std::uint16_t g_548698;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_4E0830(void)
{
    return ::g_548698;                                                       // 0x004E0830: mov 0x00548698,%ax; 0x004E0836: ret
}

} // extern "C"

#undef NATIVE_DECL
