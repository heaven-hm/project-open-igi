// Native: Core_CopyState_4CEF30; retail VA 0x004CEF30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004CEF30-0x004CEF3A (11 bytes, 1 block, 3 instructions: a1 eceda400 / a3 f0eda400 / c3).
// Tools/evidence: radare2 6.2.0 pd; target of the tail jump at 0x00402C36 from the startup init sequence 0x00402930 (evidence section 19).
// Behavior/CFG: copies the dword at 0x00A4EDEC to 0x00A4EDF0 and returns; no branches, no calls, no other memory access.
// Status/limits: STRUCTURALLY_VERIFIED (1 block / 0 edges matching retail, emitted instructions identical); runtime UNOBSERVED. The caller chain does not read the value left in EAX, so the ABI is declared void while the retail result register keeps the copied dword.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A4EDEC (read) and 0x00A4EDF0 (written) are consecutive words in retail's zeroed BSS (radare2 reports both as 0 on
// the artifact), with the written word exactly four bytes after the read one. No translation unit owns them, so they are
// defined here as one zero-initialised pair - as a pair, because the +4 relation between the two addresses is part of the
// retail layout and separate globals would let the linker order them the other way round.
std::uint32_t g_a4edec_pair[2];

static_assert(sizeof(g_a4edec_pair) == 8, "the retail pair at 0x00A4EDEC spans two consecutive dwords");

void NATIVE_DECL Core_CopyState_4CEF30()
{
    std::uint32_t const source = ::g_a4edec_pair[0];   // 0x004CEF30: mov eax, dword [0xA4EDEC]
    ::g_a4edec_pair[1] = source;                       // 0x004CEF35: mov dword [0xA4EDF0], eax
}                                              // 0x004CEF3A: ret

} // extern "C"

#undef NATIVE_DECL
