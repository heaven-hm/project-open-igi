// Native: Core_GetGlobal16_460BE0; retail VA 0x00460BE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00460BE0-0x00460BE6 (7 bytes, 1 block, 2 instructions: 66 a1 f8 da 5b 00 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; afbj reports 1 block / 2 instructions and the raw bytes decode as `mov ax, word [0x005BDAF8]` then ret. No other translation unit owns 0x005BDAF8; the retail image holds 0.
// Behavior/CFG: returns the word at 0x005BDAF8; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Only AX is written, so EAX bits 16-31 are the caller's; the declared return is word-sized.
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

std::uint16_t g_5bdaf8 = 0U; // retail 0x005BDAF8: the word this native reads (image value 0)
std::uint16_t NATIVE_DECL Core_GetGlobal16_460BE0()
{
    NATIVE_LOG_ENTER("Core_GetGlobal16_460BE0", "");
    return ::g_5bdaf8;   // 0x00460BE0: `mov ax, word [0x005BDAF8]`
}                        // 0x00460BE6: ret

} // extern "C"

#undef NATIVE_DECL
