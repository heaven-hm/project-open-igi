// Native: Keyboard_WinProc (placeholder until VERIFIED); retail VA 0x00490120-0x00490172 (83 bytes, 5 blocks, message jump table 0x00490174).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP disassembly + radare2 6.2.0 full bytes + angr CFGFast; DINPUT-CALLBACKS packet §3.1 (table 0x490174: 0x490135/0x490154/0x49016D/0x49016D/0x490135/0x490154).
// Behavior/CFG: index = msg-0x100; >5 -> return 1. WM_KEYDOWN/WM_SYSKEYDOWN: value = 0x490190(wParam,lParam); byte[0xC28E80+value] = 0x80. WM_KEYUP/WM_SYSKEYUP: same index, byte = 0. WM_CHAR/WM_SYSCHAR and all other messages: return 1. hwnd unused.
// Status/limits: TRANSCRIBED; single callee 0x490190 Keyboard_VkToStateIndex (transcribed, engine/src/retail/input/native_490190.cpp); key-state block is retail zero-fill data. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

std::int32_t Keyboard_VkToStateIndex(std::int32_t vk, std::int32_t lParam);

std::int32_t Keyboard_WinProc(void* /*hwnd*/, std::uint32_t msg, std::uint32_t wParam,
                                     std::int32_t lParam)
{
    // 0x00490120-0x0049012C: eax = msg-0x100; cmp eax,5; ja 0x0049016D.
    if (msg - 0x100U > 5U)
    {
        return 1; // 0x0049016D
    }
    switch (msg)
    {
    // 0x00490135 / 0x00490154: WM_KEYDOWN 0x0100 and WM_SYSKEYDOWN 0x0104 -> state 0x80.
    case 0x0100:
    case 0x0104:
    {
        const std::int32_t value = Keyboard_VkToStateIndex(static_cast<std::int32_t>(wParam), lParam);
        g_c28e80[value] = 0x80U;
        return 1;
    }
    // 0x00490154: WM_KEYUP 0x0101 and WM_SYSKEYUP 0x0105 -> state 0.
    case 0x0101:
    case 0x0105:
    {
        const std::int32_t value = Keyboard_VkToStateIndex(static_cast<std::int32_t>(wParam), lParam);
        g_c28e80[value] = 0U;
        return 1;
    }
    // 0x0049016D: WM_CHAR 0x0102, WM_SYSCHAR 0x0103 and default.
    default:
        return 1;
    }
}

} // extern "C"
