#include "dev_log.hpp"
#include "igi/retail/core/native_0048f970.hpp"
#include "igi/retail/core/frame_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
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

// Relocated retail WinProc chain container (0x005C8DF0 head, 0x005C8DF4
// terminator cell, 0x005C8DF8 tail pointer-to-pointer) and the 16-node pool at
// 0x005C8D28. The three container words must stay contiguous in this order:
// HeapList_Insert computes container+4 / container+8, exactly as retail.
igi::retail::core::WindowHandlerEntry** WindowHandlerTail = nullptr;
std::uintptr_t WindowHandlerTerminator = 0U;
igi::retail::core::WindowHandlerEntry* WindowHandlerChain = nullptr;
igi::retail::core::WindowHandlerEntry WindowHandlerPool[16] = {};
// Loop_StatusByte is the retail 0x005C8DE8 global that Loop_SetStatusByte owns;
// frame_natives.hpp declares it and frame_natives.cpp defines it, so this unit
// only links against it. Defining it here too broke the mingw link with
// "multiple definition of `Loop_StatusByte'".
std::uint32_t DebugKeyFlag = 0U;

// Native: WindowProcedure; retail VA 0x0048F970.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F970..0x0048FA69.
// Behavior/CFG: reads the loop status byte at 0x005C8DE8 and, when it is set, walks the handler chain at 0x005C8DF0. The global points straight at the first node, each node is twelve bytes with the next node at +0x00 and the handler at +0x08, and the procedure calls the first node's own handler, then advances to the next node and repeats while the next node has a successor of its own. The handler is called with the window, the message, wParam and lParam, and a zero result ends the procedure. The retail's own handling follows: WM_ACTIVATE returns zero, WM_CREATE returns zero, WM_DESTROY clears the loop status byte, shows the cursor, runs the close native at 0x004010B0 and posts the quit message, WM_KEYUP clears the debug key flag at 0x005C8E04 when F12 is released, WM_KEYDOWN runs the debug key handler at 0x0048F0A0 when 's' is pressed, and every other message goes to DefWindowProcA.
// Status/limits: BUILD_VERIFIED. The chain walk (head load at 0x5c8df0, head's own next pointer as the first entry, head's own handler at +8 called first, advance while the next pointer is non-null), the four handler arguments, the WM_ACTIVATE/WM_CREATE/WM_DESTROY/WM_KEYUP/WM_KEYDOWN message constants, the F12 (0x12) and 's' (0x73) key tests, the DebugKeyFlag global at 0x5c8e04 and the DefWindowProcA default were re-read from the retail's own bytes (r2, 0x0048f970..0x0048fa6f) and match. Four retail effects are deliberately NOT reproduced, because the natives they call are not transcribed in this branch; they are listed here rather than faked, and they keep this native below VERIFIED: 1. WM_ACTIVATE (0x1c) calls 0x00498460 at 0x0048f9ea before returning zero. 0x00498460 (Display_ClearSurfaces_16) has no implementation. 2. WM_DESTROY (0x2) calls 0x004010B0 at 0x0048f9d4 between ShowCursor and PostQuitMessage. 0x004010B0 has no implementation. 3. WM_KEYDOWN 's' with DebugKeyFlag set calls 0x0048F0A0(2, 0) at 0x0048fa45 and feeds its result to PostMessageA at 0x0048fa4b. 0x0048F0A0 has no implementation. 4. The chain handler's own effects are whatever the registered handlers do; none are registered by this unit. The behavioral oracle exercises the walk and the message dispatch against the retail's bytes, but its case set does not reach those three calls, so a green behavioral run does not close them.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
std::intptr_t NATIVE_DECL WindowProcedure(void* window, std::uint32_t message,
                                              std::uintptr_t wparam, std::intptr_t lparam)
{
    NATIVE_LOG_ENTER("WindowProcedure", "0x0048F970");
    if (Loop_StatusByte != 0U)
    {
        igi::retail::core::WindowHandlerEntry* const chain_head = WindowHandlerChain;
        if (chain_head != nullptr)
        {
            // The retail loads the chain head from the global at 0x005C8DF0,
            // takes the head's own next pointer as the entry, and calls the
            // head's own handler at +0x08 first. It advances to the entry and
            // repeats while the entry's own next pointer is not null, so the
            // chain runs only when the head has a successor.
            igi::retail::core::WindowHandlerEntry* entry = chain_head->next;
            if (entry != nullptr)
            {
                igi::retail::core::WindowHandlerEntry* caller = chain_head;
                for (;;)
                {
                    if (caller->handler == nullptr)
                    {
                        break;
                    }
                    if (caller->handler(window, message, wparam, lparam) == 0)
                    {
                        return 0;
                    }
                    caller = entry;
                    entry = entry->next;
                    if (entry == nullptr)
                    {
                        break;
                    }
                }
            }
        }
    }

    switch (message)
    {
    case WM_ACTIVATE_RETAIL:
        // GAP: the retail calls 0x00498460 here (0x0048f9ea); that native has no
        // implementation in this branch, so the call is omitted rather than
        // stubbed. The return value is the retail's zero.
        return 0;
    case WM_CREATE_RETAIL:
        return 0;
    case WM_DESTROY_RETAIL:
    {
        Loop_StatusByte = 0U;
#if defined(_WIN32)
        ShowCursor(TRUE);
        // GAP: the retail calls 0x004010B0 between ShowCursor and the quit post
        // (0x0048f9d4); that native has no implementation in this branch.
        PostQuitMessage(0);
#endif
        return 0;
    }
    case WM_KEYUP_RETAIL:
        // The retail's key-up arm clears the debug flag only for F12 and returns
        // zero on both of its branches (0x0048fa19 -> 0x0048fa21 and
        // 0x0048fa19 -> 0x0048fa69): it never reaches DefWindowProcA.
        if (wparam == VIRTUAL_KEY_F12)
        {
            DebugKeyFlag = 0U;
        }
        return 0;
    case WM_KEYDOWN_RETAIL:
        // The retail's key-down arm answers every key with zero: F12 sets the
        // debug flag at 0x0048fa5f, 's' with the flag set calls 0x0048F0A0 and
        // posts the message at 0x0048fa45, and every other key falls through to
        // 0x0048fa69; all of those paths return zero, none reaches
        // DefWindowProcA.
        if (wparam == VIRTUAL_KEY_F12)
        {
            DebugKeyFlag = 1U;
        }
        return 0;
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

} // extern "C"

#undef NATIVE_DECL
