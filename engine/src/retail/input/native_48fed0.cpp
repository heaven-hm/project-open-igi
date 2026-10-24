// Native: Mouse_WinProc (placeholder until VERIFIED); retail VA 0x0048FED0-0x0048FF77 (168 bytes, 10 blocks, message jump table 0x0048FF78).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP disassembly + radare2 6.2.0 full bytes + angr CFGFast; DINPUT-CALLBACKS packet §3.3 (table 0x48FF78: 0x48FF18/0x48FF48/0x48FF72/0x48FF28/0x48FF58/0x48FF72/0x48FF38/0x48FF68/0x48FF72/0x48FEE9).
// Behavior/CFG: msg-0x201 range check; WM_LBUTTONDOWN/UP -> g_5C8E10 1/0, WM_RBUTTONDOWN/UP -> g_5C8E0C 1/0, WM_MBUTTONDOWN/UP -> g_5C8E18 1/0, the up-only messages return 1, WM_MOUSEWHEEL adds (int16)(wParam>>16)/120 to g_5C8E08, everything else returns 1; hwnd is ignored.
// Status/limits: TRANSCRIBED; no callees; window messages are the retail ids; hwnd argument unused as in retail. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

std::int32_t Mouse_WinProc(void* /*hwnd*/, std::uint32_t msg, std::uint32_t wParam,
                                  std::int32_t /*lParam*/)
{
    // 0x0048FED0-0x0048FEDC: eax = msg; eax -= 0x201; if (unsigned)eax > 9 -> default return 1.
    const std::uint32_t index = msg - 0x201U;
    if (index > 9U)
    {
        return 1;
    }
    switch (msg)
    {
    // 0x0048FEE9-0x0048FF17: wheel delta, magic-divide by 120 (truncating), accumulate.
    case 0x020A: // WM_MOUSEWHEEL
    {
        const std::int32_t delta = static_cast<std::int16_t>((wParam >> 16) & 0xFFFFU);
        g_5c8e08 += delta / 120;
        return 1;
    }
    // 0x0048FF18: WM_LBUTTONDOWN
    case 0x0201:
        g_5c8e10 = 1;
        return 1;
    // 0x0048FF48: WM_LBUTTONUP
    case 0x0202:
        g_5c8e10 = 0;
        return 1;
    // 0x0048FF72: WM_LBUTTONDBLCLK (return 1 only)
    case 0x0203:
        return 1;
    // 0x0048FF28: WM_RBUTTONDOWN
    case 0x0204:
        g_5c8e0c = 1;
        return 1;
    // 0x0048FF58: WM_RBUTTONUP
    case 0x0205:
        g_5c8e0c = 0;
        return 1;
    // 0x0048FF72: WM_RBUTTONDBLCLK (return 1 only)
    case 0x0206:
        return 1;
    // 0x0048FF38: WM_MBUTTONDOWN
    case 0x0207:
        g_5c8e18 = 1;
        return 1;
    // 0x0048FF68: WM_MBUTTONUP
    case 0x0208:
        g_5c8e18 = 0;
        return 1;
    // 0x0048FF72: WM_MBUTTONDBLCLK (return 1 only)
    case 0x0209:
        return 1;
    }
    return 1; // 0x0048FF72
}

} // extern "C"
