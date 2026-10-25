// Native: Sys_GetWord_509880; retail VA 0x00509880 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509880-0x00509885 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet.
// Behavior/CFG: returns the dword at 0x00A7B2D8; no branch, no call, no store, no argument.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The word at 0x00A7B2D8 has no other owner in the tree, so it is declared here and starts zeroed.
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

// 0x00A7B2D8: the word this getter returns.
std::uint32_t g_a7b2d8;

std::uint32_t NATIVE_DECL Sys_GetWord_509880(void)
{
    NATIVE_LOG_ENTER("Sys_GetWord_509880", "0x00509880");
    return ::g_a7b2d8;   // 0x00509880: mov eax, dword [0x00A7B2D8]
}   // 0x00509885: ret

} // extern "C"

#undef NATIVE_DECL
