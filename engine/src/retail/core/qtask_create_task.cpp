#include "igi/retail/core/app_startup_natives.hpp"
#include "igi/retail/core/class_registration.hpp"
#include "igi/retail/core/flow.hpp"
#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/script/qvm_native_loader.hpp"

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

#include <cstdint>

namespace
{

// Port-owned stand-in for the retail window handle at 0x005C8BC4. Retail main
// stores its HWND there through 0x0048F0E0 at 0x0048F7A0, before it calls
// 0x00401000. The candidate main (outside this change) keeps its HWND in a
// translation-unit-local variable and writes no such global, so this copy has
// no writer yet and the SetWindowTextA branch below is not taken. The title
// store is still performed, matching the retail's unconditional store at
// 0x0048F10C.
#if defined(_WIN32)
HWND g_retail_window = nullptr;
#endif

} // namespace

// Port-owned copy of the 32-bit startup-mode flag at 0x005C8C08. Retail main
// sets it to 1 at 0x0048F656 (call to 0x0048F210) before it calls 0x00401000
// at 0x0048F7AA. The candidate has no writer for it yet, so the copy starts at
// that evidenced post-startup value; this is a scoped startup-state
// reconstruction, not an observation of a running candidate. It has external
// linkage so a later transcription of the retail writer 0x0048F210 can bind to
// it and so the compiler keeps the retail branch instead of constant-folding it.
extern "C"
{
    std::int32_t AppContext_StartupMode = 1;
}

namespace
{

// Retail 0x0048F100 (SetAppTitle): stores the argument in the window-text
// pointer at 0x005C8BD4 and, when the handle at 0x005C8BC4 is non-null, calls
// SetWindowTextA(handle, argument). The argument here is the retail string at
// 0x00536144 ("IGI"). This helper is internal to 0x00401000's reconstruction;
// it is not offered as a separately verified native.
void SetAppTitle(char const* title) noexcept
{
    AppTitle = title;
#if defined(_WIN32)
    if (g_retail_window != nullptr)
    {
        ::SetWindowTextA(g_retail_window, title);
    }
#endif
}

// Retail 0x0048F330: stores its argument in the force-precompiled flag at
// 0x005C8BF0 and leaves it in EAX. The port already owns that flag and its
// setter, which Qvm_Load reads through Script_GetForcePrecompiledState.
void SetForcePrecompiled(std::int32_t enabled) noexcept
{
    igi::retail::script::Script_SetForcePrecompiledFlag(enabled);
}

// Retail 0x0048F220: loads the 32-bit flag at 0x005C8C08 and returns it. The
// corresponding writer 0x0048F210 is not transcribed in this translation unit;
// only the read side 0x00401000 needs is provided here.
std::int32_t GetStartupMode() noexcept
{
    return AppContext_StartupMode;
}

} // namespace

// Native: QTask_CreateTask; retail VA 0x00401000.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 and angr CFGFast on retail IGI.EXE; VA 0x00401000..0x00401042 (67 bytes, 3 blocks, 2 direct edges), bytes 68 44 61 53 00 e8 f6 e0 08 00 6a 01 e8 1f e3 08 00 e8 1a 19 00 00 6a 00 e8 e3 17 00 00 83 c4 0c e8 fb e1 08 00 85 c0 75 0d 6a 04 e8 70 18 00 00 83 c4 04 b0 01 c3 6a 02 e8 63 18 00 00 83 c4 04 b0 01 c3.
// Behavior/CFG: calls 0x0048F100 with the "IGI" string at 0x00536144, calls 0x0048F330(1) to set the force-precompiled flag at 0x005C8BF0, calls QTask_RegisterClasses at 0x00402930, calls Flow_CreateHandler at 0x00402800 with null, reads the startup-mode flag at 0x005C8C08 through 0x0048F220, and calls Flow_SetState at 0x004028A0 with 2 when it is non-zero or 4 when it is zero; both paths then set AL to 1 and return, so the observed int32 result is 1.
// Status/limits: BUILD_VERIFIED; the linked symbol now exists, but QTask_RegisterClasses still reports its untranscribed registrars and throws before the flow calls, the candidate main does not call 0x00401000, and no paired retail/candidate runtime observation exists.
extern "C" int IGI_RETAIL_CDECL QTask_CreateTask()
{
    SetAppTitle("IGI");
    SetForcePrecompiled(1);
    QTask_RegisterClasses();
    Flow_CreateHandler(nullptr);

    // Retail 0x00401025..0x00401042: test the startup flag, call Flow_SetState
    // with 2 (non-zero) or 4 (zero), then `mov al, 1; retn`. The low byte of
    // Flow_SetState's EAX result is replaced, exactly as the retail does.
    const std::uint32_t state_result = (GetStartupMode() != 0) ? Flow_SetState(2U) : Flow_SetState(4U);
    return static_cast<std::int32_t>((state_result & 0xFFFFFF00U) | 1U);
}

#undef IGI_RETAIL_CDECL
