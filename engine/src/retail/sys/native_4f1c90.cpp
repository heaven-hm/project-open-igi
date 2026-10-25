// Native: Sys_InitFloatPair_4F1C90; retail VA 0x004F1C90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1C90-0x004F1CA9 (26 bytes, 1 block, 0 edges, 5 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (c7 05 f0 6b a7 00 00 00 00 00 c7 05 f4 6b a7 00 00 00 40 b8 f0 6b a7 00 c3); nine nop bytes follow.
// Behavior/CFG: stores 0 at 0x00A76BF0 and 0x40000000 at 0x00A76BF4, then returns the address 0x00A76BF0 in eax. No arguments, no branch, no callee.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. 0x40000000 is written as a raw dword by an immediate store, so the transcription stores the same bit pattern; the two dwords are the entire extent this routine touches, and no other file in the tree owns 0x00A76BF0, so this file defines the 8-byte object. What the pair means is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A76BF0: the 8-byte object this routine initialises and returns. No other owner in the tree; extent from the two stores.
std::uint8_t g_a76bf0[8];

void* NATIVE_DECL Sys_InitFloatPair_4F1C90(void)
{
    *reinterpret_cast<std::uint32_t*>(&::g_a76bf0[0]) = 0U;             // 0x004F1C90: movl bashx0,0x00A76BF0
    *reinterpret_cast<std::uint32_t*>(&::g_a76bf0[4]) = 0x40000000U;    // 0x004F1C9A: movl bashx40000000,0x00A76BF4
    return static_cast<void*>(&::g_a76bf0[0]);                          // 0x004F1CA4: mov eax,0x00A76BF0; 0x004F1CA9: ret
}

} // extern "C"

#undef NATIVE_DECL
