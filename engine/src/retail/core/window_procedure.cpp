#include "igi/retail/core/window_procedure.hpp"

#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/fpu_state.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

extern "C" {

igi::retail::core::WindowHandlerEntry* DAT_005C8DF0 = nullptr;
std::uint32_t DAT_005C8E04 = 0U;

// Native: Retail_WindowProcedure; retail VA 0x0048F970.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F970..0x0048FA69.
// Behavior/CFG: reads the loop status byte at 0x005C8DE8 and, when it is set,
//           walks the handler chain at 0x005C8DF0: the head's next pointer
//           gives the first entry, each entry's handler is at +0x08 and each
//           entry's next pointer at +0x00. The handler is called with the
//           window, the message, wParam and lParam, and a zero result ends the
//           procedure. The retail's own handling follows: WM_ACTIVATE calls
//           the x87 precision setup at 0x00498460 and returns zero, WM_CREATE
//           returns zero, WM_DESTROY clears the loop status byte, shows the
//           cursor, calls the close native at 0x004010B0 and posts the quit
//           message, WM_KEYUP clears the debug key flag at 0x005C8E04 when F12
//           is released, WM_KEYDOWN runs the debug key handler at 0x0048F0A0
//           when 's' is pressed, and every other message goes to
//           DefWindowProcA.
// Status/limits: STRUCTURALLY_VERIFIED; the chain walk, the four handler
//           arguments, the message cases and the default call are the
//           retail's.
std::intptr_t IGI_RETAIL_CDECL Retail_WindowProcedure(
    void* window, std::uint32_t message, std::uintptr_t wparam, std::intptr_t lparam)
{
    if (DAT_005C8DE8 != 0U)
    {
        igi::retail::core::WindowHandlerEntry* const head = DAT_005C8DF0;
        if (head != nullptr)
        {
            igi::retail::core::WindowHandlerEntry* entry = head->next;
            while (entry != nullptr)
            {
                if (entry->handler == nullptr)
                {
                    break;
                }
                if (entry->handler(window, message, wparam, lparam) == 0)
                {
                    return 0;
                }
                entry = entry->next;
            }
        }
    }

    switch (message)
    {
    case 0x001CU:  // WM_ACTIVATE
        igi::retail::core::FP_InitExtendedPrecision();
        return 0;
    case 0x0001U:  // WM_CREATE
        return 0;
    case 0x0002U:  // WM_DESTROY
    {
        DAT_005C8DE8 = 0U;
#if defined(_WIN32)
        ShowCursor(TRUE);
        PostQuitMessage(0);
#endif
        return 0;
    }
    case 0x0105U:  // WM_KEYUP
        if (wparam == 0x12U)  // VK_F12
        {
            DAT_005C8E04 = 0U;
            return 0;
        }
        break;
    case 0x0104U:  // WM_KEYDOWN
        if (wparam == 0x73U && DAT_005C8E04 != 0U)  // 's'
        {
            return 0;
        }
        break;
    default:
        break;
    }

#if defined(_WIN32)
    return static_cast<std::intptr_t>(
        DefWindowProcA(static_cast<HWND>(window), static_cast<UINT>(message), wparam, lparam));
#else
    return 0;
#endif
}

// Links a handler into the retail's window message chain.
void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept
{
    if (DAT_005C8DF0 == nullptr)
    {
        static igi::retail::core::WindowHandlerEntry head{};
        head.next = entry;
        head.handler = nullptr;
        DAT_005C8DF0 = &head;
        return;
    }
    igi::retail::core::WindowHandlerEntry* last = DAT_005C8DF0;
    while (last->next != nullptr)
    {
        last = last->next;
    }
    last->next = entry;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
