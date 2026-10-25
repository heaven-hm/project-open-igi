// Native: Core_GetGlobal8_460C40; retail VA 0x00460C40 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00460C40-0x00460C45 (6 bytes, 1 block, 2 instructions: a0 17 f3 53 00 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; afbj reports 1 block / 2 instructions and the raw bytes decode as `mov al, byte [0x0053F317]` then ret. Third byte of the 0x0053F314-0x0053F317 block: 0x00460BF0 owns 0x0053F314, 0x00460C10 owns 0x0053F315, and this getter reads 0x0053F317.
// Behavior/CFG: returns the byte at 0x0053F317; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Only AL is written, so a C++ byte return zero-extends into EAX where retail leaves the upper bits as the caller left them; the byte result is identical for every AL/uint8_t consumer.
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

std::uint8_t g_53f317 = 0xFFU; // retail 0x0053F317: the byte this native reads (image value 0xFF)
std::uint8_t NATIVE_DECL Core_GetGlobal8_460C40()
{
    NATIVE_LOG_ENTER("Core_GetGlobal8_460C40", "0x00460C40");
    return ::g_53f317;   // 0x00460C40: `mov al, byte [0x0053F317]`
}                        // 0x00460C45: ret

} // extern "C"

#undef NATIVE_DECL
