// Native: Core_GetGlobal8_47E990; retail VA 0x0047E990 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0047E990-0x0047E995 (6 bytes, 1 block, 2 instructions: a0 14 09 54 00 / c3).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; afbj reports 1 block / 2 instructions and the raw bytes decode as `mov al, byte [0x00540914]` then ret. This is the byte that the FlashBang registrar 0x0047E700 allocates and writes (native_0047e700.cpp), so this file DECLARES it extern and does not define it - one definition per retail address.
// Behavior/CFG: returns the byte at 0x00540914; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Only AL is written, so a C++ byte return zero-extends into EAX where retail leaves the upper bits as the caller left them; the byte result is identical for every AL/uint8_t consumer. The byte itself is the event id allocated by 0x0047E700.
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

extern std::uint8_t g_540914; // retail 0x00540914: defined in native_0047e700.cpp, which is its only writer; this routine reads it
std::uint8_t NATIVE_DECL Core_GetGlobal8_47E990()
{
    NATIVE_LOG_ENTER("Core_GetGlobal8_47E990", "0x0047E990");
    return ::g_540914;   // 0x0047E990: `mov al, byte [0x00540914]`
}                        // 0x0047E995: ret

} // extern "C"

#undef NATIVE_DECL
