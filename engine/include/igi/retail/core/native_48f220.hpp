#pragma once

// Native at retail VA 0x0048F220.
//
// `mov eax,[0x005C8C08]; ret` -- one block, 6 bytes. Reads the 32-bit
// startup-mode flag. Its writer is the separate native at 0x0048F210
// (Game_SetStartupMode), whose storage this file binds to rather than copies.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail global at VA 0x005C8C08, owned by the writer 0x0048F210.
extern std::int32_t AppContext_StartupMode;

// Native at VA 0x0048F220.
std::int32_t NATIVE_DECL GetStartupMode();

} // extern "C"

#undef NATIVE_DECL
