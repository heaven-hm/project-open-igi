#pragma once

// Retail start-up setters reconstructed in engine/src/retail/core/app_context_extra.cpp:
// SetAppTitle 0x0048F100 (writes the document-title pointer 0x005C8BD4 and forwards
// the title to SetWindowTextA when the window handle exists) and
// SetDebugCompileState 0x0048F1C0 (sign-extends the argument into 0x005C8BEC).

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {
std::int32_t NATIVE_DECL SetAppTitle(char const* title);
std::int32_t NATIVE_DECL SetDebugCompileState(std::int32_t value);
}

#undef NATIVE_DECL
