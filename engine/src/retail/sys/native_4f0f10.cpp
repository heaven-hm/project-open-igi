// Native: Sys_GetGlobal32_4F0F10; retail VA 0x004F0F10 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F0F10-0x004F0F15 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm 3.01 over the raw bytes (a1 8c 0c a7 00 c3), which agree on both instructions; `px 4` at
//   0x00A70C8C gives zero, and the address is past `.data`'s raw extent (raw ends 0x00552000), so the object is zero fill in the image.
// Behavior/CFG: loads the four-byte value at 0x00A70C8C into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Four bytes is the width this load proves; the value's meaning is not established. This getter is
//   one of the two argument-4 sources of the four 0x004F1A80 appends in core/native_45e450.cpp (frame offset 0xF8) and is read by 0x0044C8D0.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A70C8C: the four-byte object this getter reads; nothing else in the tree owns it yet.
std::uint32_t g_a70c8c;

std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F10(void)
{
    return ::g_a70c8c;                                                       // 0x004F0F10: mov 0x00a70c8c,%eax; 0x004F0F15: ret
}

} // extern "C"

#undef NATIVE_DECL
