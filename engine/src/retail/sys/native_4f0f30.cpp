// Native: Sys_GetGlobal32_4F0F30; retail VA 0x004F0F30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0F30-0x004F0F36 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (a1 a4 0c a7 00 c3).
// Behavior/CFG: loads the four-byte value at 0x00A70CA4 into eax and returns it; no argument, no branch, no store, no call.
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

// 0x00A70CA4: the four-byte object this getter reads; nothing else in the tree owns it yet.
std::uint32_t g_a70ca4;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F30(void)
{
    return ::g_a70ca4;                                                       // 0x004F0F30: mov 0x00a70ca4,%eax; 0x004F0F35: ret
}

} // extern "C"

#undef NATIVE_DECL
