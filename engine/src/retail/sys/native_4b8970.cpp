// Native: Sys_SetItemWord_4B8970; retail VA 0x004B8970 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8970-0x004B897A (10 bytes, 1 block, 0 edges, 3 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (8b 44 24 04 a3 98 65 a9 00 c3); 0x00A96598 has five references in the image (0x004B7E10, 0x004B86A5, 0x004B898D, 0x004B89D4 and this store), all four-byte accesses.
// Behavior/CFG: stores the single cdecl argument at 0x00A96598 and returns; straight-line, no branch, no load, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The dword's meaning is not established; four bytes is the extent every reference in the image uses, and this file defines the object because nothing else in the tree owns it.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A96598: the stored dword. Extent from the uniform four-byte access in all five references.
std::uint32_t g_a96598;

void NATIVE_DECL Sys_SetItemWord_4B8970(std::int32_t value)
{
    NATIVE_LOG_ENTER("Sys_SetItemWord_4B8970", "0x004B8970");
    ::g_a96598 = static_cast<std::uint32_t>(value);                      // 0x4B8974: mov %eax,0x00A96598
}                                                                      // 0x4B8979: ret

} // extern "C"

#undef NATIVE_DECL
