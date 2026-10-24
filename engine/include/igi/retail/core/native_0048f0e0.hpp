#pragma once

// The retail window handle global and the native that sets it.
//
// igi.exe stores the window handle at 0x005C8BC4 during start-up. The native
// at 0x0048F0E0 writes it, and the native at 0x0048F100 reads it and calls
// SetWindowTextA with the application title, so the global is the window
// handle.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// The retail's window handle at 0x005C8BC4.
extern void* g_window_handle;

// Native: SetWindowHandle() - VA 0x0048F0E0 - complete port.
// Behavior: one basic block loads the 32-bit argument, stores it at the
//           window handle global 0x005C8BC4, and returns the same bits in EAX.
void* NATIVE_DECL SetWindowHandle(void* window);

} // extern "C"

#undef NATIVE_DECL
