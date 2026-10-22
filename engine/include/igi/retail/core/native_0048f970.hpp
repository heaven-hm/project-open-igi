#pragma once

// The retail window procedure at 0x0048F970 and its handler chain.
//
// igi.exe registers this procedure as the window class's lpfnWndProc. The
// procedure walks the handler chain at 0x005C8DF0 before it applies the
// retail's own message handling, so the game's input and event handlers see
// every message first.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_STDCALL __stdcall
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_STDCALL __attribute__((stdcall))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_STDCALL
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// One entry of the retail's window message chain. The retail's node is twelve
// bytes: the next entry at +0x00, the previous entry at +0x04 and the handler
// at +0x08, so the procedure reads the handler at the same offset the retail
// reads it at. Registration at 0x0048F490 zeroes +0x00/+0x04 before insertion;
// HeapList_Insert (0x004AF8F0) stores the old tail in +0x04 and
// HeapList_Remove (0x004AF960) treats +0x00/+0x04 as next/previous.
using WindowHandlerCallback = std::intptr_t (IGI_RETAIL_CDECL *)(
    void* window, std::uint32_t message, std::uintptr_t wparam, std::intptr_t lparam);

struct WindowHandlerEntry
{
    WindowHandlerEntry* next;
    WindowHandlerEntry* previous;
    WindowHandlerCallback handler;
};

} // namespace igi::retail::core

extern "C" {

// Retail global at VA 0x005C8DF0: the head of the window message chain.
extern igi::retail::core::WindowHandlerEntry* WindowHandlerChain;

// Retail global at VA 0x005C8DE8: the loop status byte the procedure reads
// before it walks the chain.
// The retail's loop status byte at 0x005C8DE8. The setter at 0x0048F480 and
// the window procedure at 0x0048F970 share it.
extern std::uint8_t Loop_StatusByte;

// Retail global at VA 0x005C8E04: the debug/Alt flag, set by WM_SYSKEYDOWN for
// VK_MENU (0x12) and cleared by WM_SYSKEYUP for VK_MENU.
extern std::uint32_t DebugKeyFlag;

// Native at VA 0x0048F970. This is a Windows window procedure (stdcall); the
// linked WindowHandlerEntry callbacks remain cdecl.
// Reads the loop status byte; when it is set it walks the handler chain and
// calls each entry's handler with the window, the message, wParam and lParam,
// ending the procedure when a handler returns zero. It then applies the
// retail's own handling. Message 0x001C is Win32 WM_ACTIVATEAPP (winuser.h;
// WM_ACTIVATE is 0x0006): the retail handles WM_ACTIVATEAPP by calling the x87
// chop-rounding setter at 0x00498460 and returning zero. WM_CREATE (0x0001)
// returns zero. WM_DESTROY (0x0002) clears the loop status byte, calls
// ShowCursor(TRUE), calls the entry at 0x004010B0, calls PostQuitMessage(0),
// and returns zero, in that order. The candidate does NOT reproduce the
// 0x004010B0 step: that 28-byte body is unreconstructed and is recorded as an
// unresolved gap rather than modelled as a no-op.
// WM_SYSKEYUP (0x0105) clears the debug/Alt flag for VK_MENU (0x12).
// WM_SYSKEYDOWN (0x0104) posts WM_DESTROY to the retail window handle for
// VK_F4 (0x73) when the flag is set; otherwise VK_MENU sets the flag. Both
// system-key messages always return zero. Other messages go to DefWindowProcA.
// Status: STRUCTURALLY_VERIFIED (structural review only); compiles for the
// MinGW x86 target; no paired retail runtime evidence. Two candidate-only null
// guards have no retail counterpart, and the WM_DESTROY call to 0x004010B0 is
// not reproduced; both are recorded in the verification manifest.
std::intptr_t IGI_RETAIL_STDCALL WindowProcedure(void* window, std::uint32_t message,
                                                std::uintptr_t wparam, std::intptr_t lparam);

// Links a handler into the retail's window message chain.
void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept;

} // extern "C"

#undef IGI_RETAIL_CDECL
#undef IGI_RETAIL_STDCALL
