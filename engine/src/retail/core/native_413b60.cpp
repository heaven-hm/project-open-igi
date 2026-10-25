// Native: Sys_GetSettingByte_413B60; retail VA 0x00413B60 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413B60-0x00413B65 (6 bytes, 1 block, 0 edges, 2 instructions: a0 b2915300 / c3).
// Tools/evidence: radare2 6.2.0 af/afij and the raw bytes decode as `mov al, byte [0x005391B2]` then `ret`. 0x005391B2 is index 6, the last byte of the seven-byte event-id block 0x005391AC..0x005391B2; its only writer in the whole image is the registrar 0x00413DF0 (stores at 0x00413DF9/0x413E05/0x413E11/0x413E1D/0x413E29/0x413E35/0x413E4D) and its readers are 0x00413B00-0x00413B60, 0x00413C87, 0x00413D5D, 0x0041464F, 0x004146BE, 0x00414722 and 0x00414789. Same shape as the accepted siblings 0x00413B20/0x00413B30/0x00413B40, one VA each.
// Behavior/CFG: returns the byte at 0x005391B2; no argument, no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Retail writes only AL and leaves EAX bits 8-31 as the caller left them, while a C++ byte return zero-extends into EAX; the byte-level result is identical for every caller that consumes AL or a std::uint8_t. The byte is read through its single owner g_5391ac (index 6) declared in engine/src/retail/core/native_413df0.cpp, because the registrar 0x00413DF0 writes that same storage: a private copy in this file would always return the image value and never the event id the registrar stores. The image holds 0x00 here, unlike the six bytes before it, and the body does not establish why.
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

// 0x005391AC: the seven-byte event-id block 0x005391AC..0x005391B2, owned by the registrar 0x00413DF0 in
// engine/src/retail/core/native_413df0.cpp (image values FF FF FF FF FF FF 00). 0x005391B2 is its index 6.
extern std::uint8_t g_5391ac[7];

std::uint8_t NATIVE_DECL Sys_GetSettingByte_413B60(void)
{
    NATIVE_LOG_ENTER("Sys_GetSettingByte_413B60", "0x00413B60");
    return ::g_5391ac[6];   // 0x00413B60: mov al, byte [0x005391B2]
}                           // 0x00413B65: ret

} // extern "C"

#undef NATIVE_DECL
