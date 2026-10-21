#include "igi/retail/core/native_0048f970.hpp"

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

namespace {

// The retail's window messages the procedure handles itself.
constexpr std::uint32_t WM_ACTIVATE_RETAIL = 0x001CU;
constexpr std::uint32_t WM_CREATE_RETAIL = 0x0001U;
constexpr std::uint32_t WM_DESTROY_RETAIL = 0x0002U;
constexpr std::uint32_t WM_KEYDOWN_RETAIL = 0x0104U;
constexpr std::uint32_t WM_KEYUP_RETAIL = 0x0105U;

// The virtual keys the retail's debug handling watches.
constexpr std::uintptr_t VIRTUAL_KEY_S = 0x73U;
constexpr std::uintptr_t VIRTUAL_KEY_F12 = 0x12U;

} // namespace

extern "C" {

igi::retail::core::WindowHandlerEntry* WindowHandlerChain = nullptr;
std::uint8_t LoopStatusByte = 0U;
std::uint32_t DebugKeyFlag = 0U;

// Native: WindowProcedure; retail VA 0x0048F970.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F970..0x0048FA69.
// Behavior/CFG: reads the loop status byte at 0x005C8DE8 and, when it is set, walks the handler chain at 0x005C8DF0: the head's next pointer gives the first entry, each entry's handler is at +0x08 and each entry's next pointer at +0x00. The handler is called with the window, the message, wParam and lParam, and a zero result ends the procedure. The retail's own handling follows: WM_ACTIVATE returns zero, WM_CREATE returns zero, WM_DESTROY clears the loop status byte, shows the cursor, runs the close native at 0x004010B0 and posts the quit message, WM_KEYUP clears the debug key flag at 0x005C8E04 when F12 is released, WM_KEYDOWN runs the debug key handler at 0x0048F0A0 when 's' is pressed, and every other message goes to DefWindowProcA.
// Status/limits: STRUCTURALLY_VERIFIED; the chain walk, the four handler arguments, the message cases and the default call are the retail's. The close native at 0x004010B0 and the debug key handler at 0x0048F0A0 are not transcribed in this branch yet, so those two calls are left unimplemented rather than faked.
std::intptr_t IGI_RETAIL_CDECL WindowProcedure(void* window, std::uint32_t message,
                                              std::uintptr_t wparam, std::intptr_t lparam)
{
    if (LoopStatusByte != 0U)
    {
        igi::retail::core::WindowHandlerEntry* const chain_head = WindowHandlerChain;
        if (chain_head != nullptr)
        {
            igi::retail::core::WindowHandlerEntry* entry = chain_head->next;
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
    case WM_ACTIVATE_RETAIL:
        return 0;
    case WM_CREATE_RETAIL:
        return 0;
    case WM_DESTROY_RETAIL:
    {
        LoopStatusByte = 0U;
#if defined(_WIN32)
        ShowCursor(TRUE);
        PostQuitMessage(0);
#endif
        return 0;
    }
    case WM_KEYUP_RETAIL:
        if (wparam == VIRTUAL_KEY_F12)
        {
            DebugKeyFlag = 0U;
            return 0;
        }
        break;
    case WM_KEYDOWN_RETAIL:
        if (wparam == VIRTUAL_KEY_S && DebugKeyFlag != 0U)
        {
            return 0;
        }
        break;
    default:
        break;
    }

#if defined(_WIN32)
    return static_cast<std::intptr_t>(DefWindowProcA(static_cast<HWND>(window),
                                                       static_cast<UINT>(message), wparam, lparam));
#else
    return 0;
#endif
}

void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept
{
    if (WindowHandlerChain == nullptr)
    {
        static igi::retail::core::WindowHandlerEntry chain_head{};
        chain_head.next = entry;
        chain_head.handler = nullptr;
        WindowHandlerChain = &chain_head;
        return;
    }
    igi::retail::core::WindowHandlerEntry* last_entry = WindowHandlerChain;
    while (last_entry->next != nullptr)
    {
        last_entry = last_entry->next;
    }
    last_entry->next = entry;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
