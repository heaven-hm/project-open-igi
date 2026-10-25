#pragma once

// The three consecutive byte getters at retail VAs 0x00408320, 0x00408330 and
// 0x00408340. Each is one block, `mov al,byte [addr]; ret`, and each reads its
// own byte of the three-byte run at 0x00538518..0x0053851A. They differ only by
// the byte they read and by one byte of code address.
//
// All three bytes hold 0xFF in the image, so each is .data with an 0xFF initialiser,
// not a zero-filled slot. Byte 0's storage belongs to its writer, native 0x00407D60.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail 0x00538518: owned by its writer, native 0x00407D60 (render/native_00407d60.cpp).
extern std::uint8_t g_538518;
// Retail 0x00538519 and 0x0053851A: read below, no writer transcribed yet.
extern std::uint8_t g_538519;
extern std::uint8_t g_53851a;

// Native at VA 0x00408320: `mov al,byte [0x00538518]; ret`.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_408320();
// Native at VA 0x00408330: `mov al,byte [0x00538519]; ret`.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_408330();
// Native at VA 0x00408340: `mov al,byte [0x0053851a]; ret`.
std::uint8_t NATIVE_DECL Sys_GetGlobal8_408340();

} // extern "C"

#undef NATIVE_DECL
