#pragma once

// The retail window procedure at 0x0048F970 and its handler chain.
//
// igi.exe registers this procedure as the window class's lpfnWndProc. The
// procedure walks the handler chain at 0x005C8DF0 before it applies the
// retail's own message handling, so the game's input and event handlers see
// every message first.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

// One entry of the retail's window message chain. The next entry is at
// +0x00 and the handler at +0x08.
struct WindowHandlerEntry
{
    WindowHandlerEntry* next;
    std::intptr_t (*handler)(void* window, std::uint32_t message, std::uintptr_t wparam,
                              std::intptr_t lparam);
};

} // namespace igi::retail::core

extern "C" {

// Retail global at VA 0x005C8DF0: the head of the window message chain.
extern igi::retail::core::WindowHandlerEntry* WindowHandlerChain;

// Retail global at VA 0x005C8DE8: the loop status byte the procedure reads
// before it walks the chain. Loop_SetStatusByte owns this global, so the name
// and the storage come from frame_natives.hpp; declaring a second spelling here
// gave the 0x005C8DE8 byte two symbol names and broke the candidate link.
extern std::uint8_t Loop_StatusByte;

// Retail global at VA 0x005C8E04: the debug key flag, cleared when F12 is
// released.
extern std::uint32_t DebugKeyFlag;

// Native at VA 0x0048F970.
// Reads the loop status byte; when it is set it walks the handler chain and
// calls each entry's handler with the window, the message, wParam and lParam,
// ending the procedure when a handler returns zero. It then applies the
// retail's own handling: WM_ACTIVATE returns zero, WM_CREATE returns zero,
// WM_DESTROY clears the loop status byte, shows the cursor, runs the close
// native and posts the quit message, WM_KEYUP clears the debug key flag when
// F12 is released, WM_KEYDOWN runs the debug key handler when 's' is pressed,
// and every other message goes to DefWindowProcA.
std::intptr_t NATIVE_DECL WindowProcedure(void* window, std::uint32_t message,
                                              std::uintptr_t wparam, std::intptr_t lparam);

// Links a handler into the retail's window message chain.
void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept;

} // extern "C"

#undef NATIVE_DECL
