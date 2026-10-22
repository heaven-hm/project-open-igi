#include "igi/retail/core/native_0048f970.hpp"
#include "igi/retail/core/native_0048f0e0.hpp"
#include "igi/retail/core/fpu_state.hpp"

#include <cfenv>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_STDCALL __stdcall
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_STDCALL __attribute__((stdcall))
#else
#define IGI_RETAIL_STDCALL
#endif

namespace {

// The retail's window messages the procedure handles itself.
//
// Win32 WM_ACTIVATEAPP is 0x001C (winuser.h); WM_ACTIVATE is 0x0006. The
// retail compares against 0x001C and, for that value only, calls the x87
// chop-rounding setter at 0x00498460 and returns 0. The _RETAIL suffix avoids
// the windows.h macro of the same name.
constexpr std::uint32_t WM_ACTIVATEAPP_RETAIL = 0x001CU;
constexpr std::uint32_t WM_CREATE_RETAIL = 0x0001U;
constexpr std::uint32_t WM_DESTROY_RETAIL = 0x0002U;
// 0x0104 and 0x0105 are the Win32 system-key messages, WM_SYSKEYDOWN and
// WM_SYSKEYUP; the retail uses the system-key variants, not WM_KEYDOWN/WM_KEYUP.
constexpr std::uint32_t WM_SYSKEYDOWN_RETAIL = 0x0104U;
constexpr std::uint32_t WM_SYSKEYUP_RETAIL = 0x0105U;

// Retail virtual-key codes: VK_MENU (Alt)=0x12 and VK_F4=0x73.
constexpr std::uintptr_t VIRTUAL_KEY_MENU = 0x12U;
constexpr std::uintptr_t VIRTUAL_KEY_F4 = 0x73U;

// Local helper for the retail's x87 chop-rounding setter at 0x00498460,
// called unconditionally by WindowProcedure for WM_ACTIVATEAPP (0x001C).
// Retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// An objdump re-check of the exact entry (2026-10-23) confirmed the tracked
// transcription and its Binary Ninja/Radare2 evidence live in fpu_state.cpp;
// this helper exists for the window procedure's call site and is not the
// tracked transcription of 0x00498460.
// The retail body is one basic block, 0x00498460..0x00498476, with no branches
// and no stack arguments: wait; fnstcw word [0x006E5BC4]; or word
// [0x006E5BC4], 0x0C00; fldcw word [0x006E5BC4]; ret. It writes the GLOBAL
// x87 control word at 0x006E5BC4, forces both rounding control bits (bits
// 10-11) to truncate, preserves every other bit, uses no stack temporary,
// does not restore the previous control word and returns no value.
// `or 0x0C00` is exactly (CW & 0xF3FF) | 0x0C00 for every control word, so it
// sets both rounding bits and touches no other bit.
// The repository already transcribes this exact native as
// igi::retail::core::FP_InitExtendedPrecision in fpu_state.cpp, operating on the
// same retail control-word global (g_6e5bc4, retail VA 0x006E5BC4). The GNU
// branch below calls that transcription; the MSVC branch mirrors the same
// instructions because MSVC has no GNU memory-operand syntax, and that branch
// is not compile-tested here.
// This helper is TRANSCRIBED with no runtime evidence. Non-x86 hosts have no
// x87 control word or 0x006E5BC4 mirror; the host-test-only fallback below
// reproduces the rounding mode through the C floating-point environment and is
// not a bit-exact reproduction.
void SetRetailChopRounding()
{
#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
    // Retail calls 0x00498460 here. This is the repository's transcription of
    // that exact native; it writes g_6e5bc4 (retail 0x006E5BC4).
    igi::retail::core::FP_InitExtendedPrecision();
#elif defined(_MSC_VER) && defined(_M_IX86)
    // Same effect as the retail instructions; MSVC has no GNU memory-operand
    // syntax. This branch is not compile-tested in this environment (the
    // tested target is MinGW x86).
    unsigned short control_word;
    __asm
    {
        fnstcw control_word
        mov ax, control_word
        or ax, 0C00h
        mov control_word, ax
        fldcw control_word
    }
    igi::retail::core::g_6e5bc4 = control_word;
#elif defined(FE_TOWARDZERO)
    // Host-test-only fallback: reproduce only the observable rounding-mode
    // effect through the C floating-point environment.
    std::fesetround(FE_TOWARDZERO);
#endif
}

} // namespace

extern "C" {

igi::retail::core::WindowHandlerEntry* WindowHandlerChain = nullptr;
std::uint8_t Loop_StatusByte = 0U;
std::uint32_t DebugKeyFlag = 0U;

// Native: WindowProcedure; retail VA 0x0048F970.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja decompiler + Radare2 CFG + angr CFGFast on retail; docs/fidelity/evidence/0x0048F970-semantic-review.json.
// Behavior/CFG: chain walk, then 0x001C x87 chop setter, 0x0001, 0x0002 teardown, 0x0104/0x0105 Alt/F4 keys, DefWindowProcA fallback.
// Status/limits: STRUCTURALLY_VERIFIED (structural only; no runtime evidence).
// The retail chain head is the global at 0x005C8DF0, initialised to the
// sentinel cell 0x005C8DF4; each twelve-byte node has next at +0x00, previous
// at +0x04 and the handler at +0x08. The procedure calls the head's own
// handler, advances to the head's next node and repeats while that node's next
// pointer is not null. The handler is called with the window, the message,
// wParam and lParam, and a zero result ends the procedure.
// Message handling: WM_ACTIVATEAPP (0x001C) calls the x87 chop-rounding setter
// at 0x00498460 and returns zero; 0x0001 returns zero; WM_DESTROY (0x0002)
// clears the loop status byte at 0x005C8DE8, calls ShowCursor(TRUE), calls the
// unreconstructed entry at 0x004010B0, calls PostQuitMessage(0) and returns
// zero, in that order; WM_SYSKEYDOWN (0x0104) and WM_SYSKEYUP (0x0105) always
// return zero, with VK_MENU (0x12) setting/clearing DebugKeyFlag at 0x005C8E04
// and VK_F4 (0x73) posting WM_DESTROY to the window handle at 0x005C8BC4 only
// when DebugKeyFlag is nonzero; other messages go to DefWindowProcA.
// Limitations: two candidate-only null guards (the chain head and a handler)
// have no retail counterpart, and the WM_DESTROY call to 0x004010B0 is
// recorded but NOT reproduced because its body is unreconstructed. The x87
// chop-rounding call is reproduced through FP_InitExtendedPrecision. No paired
// retail runtime evidence exists; the structural comparison and every
// divergence are in 0x0048F970-structural-comparison.json and the semantic
// review named above.
std::intptr_t IGI_RETAIL_STDCALL WindowProcedure(void* window, std::uint32_t message,
                                                std::uintptr_t wparam, std::intptr_t lparam)
{
    if (Loop_StatusByte != 0U)
    {
        igi::retail::core::WindowHandlerEntry* const chain_head = WindowHandlerChain;
        // PORT SAFETY DIVERGENCE: the retail loads the head global at
        // 0x005C8DF0 and dereferences it with no null check (0x0048F989
        // mov eax,[0x005C8DF0]; 0x0048F98E mov esi,[eax]), and calls [eax+8]
        // unconditionally once that entry is nonzero. The initialiser at
        // 0x0048F68C writes 0x005C8DF4 -- the chain's sentinel/terminator
        // cell -- into 0x005C8DF0, so a valid chain head is normally non-null
        // and retail has no head guard. This check exists only so a null chain
        // in the port cannot fault; it is not retail behavior.
        if (chain_head != nullptr)
        {
            // The retail takes the head's own next pointer as the entry and
            // calls the head's own handler at +0x08 first. It advances to the
            // entry and repeats while that entry's own next pointer is not
            // null, so the chain runs only when the head has a successor.
            igi::retail::core::WindowHandlerEntry* entry = chain_head->next;
            if (entry != nullptr)
            {
                igi::retail::core::WindowHandlerEntry* caller = chain_head;
                for (;;)
                {
                    // PORT SAFETY DIVERGENCE: retail calls [eax+8]
                    // unconditionally; this null-handler check only prevents a
                    // malformed port chain from calling through null.
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
    case WM_ACTIVATEAPP_RETAIL:
        // Retail handles WM_ACTIVATEAPP by calling the x87 chop-rounding
        // setter at 0x00498460 unconditionally and returning zero.
        SetRetailChopRounding();
        return 0;
    case WM_CREATE_RETAIL:
        return 0;
    case WM_DESTROY_RETAIL:
    {
        // Retail order (0x0048F9C5..0x0048F9E7): clear the loop status byte at
        // 0x005C8DE8, call ShowCursor(TRUE), call 0x004010B0, call
        // PostQuitMessage(0), return 0.
        //
        // UNRESOLVED GAP: the 0x004010B0 step is NOT reproduced. Retail
        // 0x004010B0..0x004010CB (28 bytes) is not a no-op: it calls
        // 0x00401A60; when EAX != 0 it calls 0x00402880, pushes the result and
        // calls 0x004015F0; it then tail-jumps to 0x00402C40 with no cleanup.
        // That body is unreconstructed, so this candidate omits the call
        // rather than pretending the native does nothing. Only the surrounding
        // documented ordering is reproduced.
        Loop_StatusByte = 0U;
#if defined(_WIN32)
        ShowCursor(TRUE);
        // Retail calls 0x004010B0 here (unreconstructed; documented above).
        PostQuitMessage(0);
#endif
        return 0;
    }
    case WM_SYSKEYUP_RETAIL:
        if (wparam == VIRTUAL_KEY_MENU)
        {
            DebugKeyFlag = 0U;
        }
        return 0;
    case WM_SYSKEYDOWN_RETAIL:
        if (wparam == VIRTUAL_KEY_F4 && DebugKeyFlag != 0U)
        {
#if defined(_WIN32)
            // Retail calls the zero-argument getter at 0x0048F0A0, which is a
            // load of the window handle at 0x005C8BC4, and passes its result as
            // the message's window. The candidate reads the same global.
            PostMessageA(static_cast<HWND>(g_window_handle), WM_DESTROY_RETAIL, 0, 0);
#endif
        }
        else if (wparam == VIRTUAL_KEY_MENU)
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

void LinkWindowHandler(igi::retail::core::WindowHandlerEntry* entry) noexcept
{
    // The retail's chain is a list whose head is the first registered entry and
    // whose tail points at the list's end cell, so the head always has a
    // successor and the procedure's walk reaches every handler. The port keeps
    // the same shape with a terminal entry that carries no handler.
    static igi::retail::core::WindowHandlerEntry chain_tail{};
    if (WindowHandlerChain == nullptr)
    {
        entry->next = &chain_tail;
        WindowHandlerChain = entry;
        return;
    }
    igi::retail::core::WindowHandlerEntry* last_entry = WindowHandlerChain;
    while (last_entry->next != nullptr && last_entry->next != &chain_tail)
    {
        last_entry = last_entry->next;
    }
    last_entry->next = entry;
    entry->next = &chain_tail;
}

} // extern "C"

#undef IGI_RETAIL_STDCALL
