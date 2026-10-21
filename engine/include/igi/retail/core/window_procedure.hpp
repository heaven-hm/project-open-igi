#pragma once

// The retail window procedure and its handler chain.
//
// igi.exe registers the procedure at 0x0048F970 as the window class's
// lpfnWndProc. The procedure walks the handler chain at 0x005C8DF0 first, so
// the game's input and event handlers see every message before the retail's
// own message handling runs.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// The retail's window message handler chain entry. The next entry is at
// +0x00 and the handler at +0x08.
struct WindowHandlerEntry
{
    WindowHandlerEntry* next;
    std::intptr_t (*handler)(void* window, std::uint32_t message, std::uintptr_t wparam,
                              std::intptr_t lparam);
};

// The retail's window message arguments.
struct WindowMessage
{
    void* window;
    std::uint32_t message;
    std::uintptr_t wparam;
    std::intptr_t lparam;
};

} // namespace igi::retail::core

extern "C" {

// The retail's handler chain head at 0x005C8DF0. The window procedure reads
// the first entry from it and walks the chain through the entries' next
// pointers.
extern igi::retail::core::WindowHandlerEntry* DAT_005C8DF0;

// The retail's debug key flag at 0x005C8E04. The window procedure clears it
// when F12 is released and the debug key handler sets it when 's' is pressed.
extern std::uint32_t DAT_005C8E04;

// Native: Retail_WindowProcedure() - VA 0x0048F970 - complete port.
// Behavior: reads the loop status byte at 0x005C8DE8 and, when it is set,
// walks the handler chain at 0x005C8DF0 and calls each entry's handler with
//           the window, the message, wParam and lParam. A handler that
//           returns zero ends the procedure. The retail's own handling
//           follows: WM_ACTIVATE sets the x87 extended precision and returns
//           zero, WM_CREATE returns zero, WM_DESTROY clears the loop status
//           byte, shows the cursor, runs the close native at 0x004010B0 and
//           posts the quit message, WM_KEYUP clears the debug key flag when
//           F12 is released, WM_KEYDOWN runs the debug key handler when 's' is
//           pressed, and every other message goes to DefWindowProcA.
std::intptr_t IGI_RETAIL_CDECL Retail_WindowProcedure(
    void* window, std::uint32_t message, std::uintptr_t wparam, std::intptr_t lparam);

// Links a handler into the retail's window message chain. The chain head is
// the entry at 0x005C8DF0 and the entries are linked through their next
// pointers, the way the retail's own handlers are.
void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept;

} // extern "C"

#undef IGI_RETAIL_CDECL
