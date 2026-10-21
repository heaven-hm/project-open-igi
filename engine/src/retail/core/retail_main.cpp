#include "igi/retail/core/retail_main.hpp"

#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/fpu_state.hpp"
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

// The retail's main keeps the window handle in its frame; the per-frame loop
// destroys it when the loop status byte is cleared.
#if defined(_WIN32)
HWND g_main_window = nullptr;
#endif

} // namespace

extern "C" {

// Native: Main; retail VA 0x0048F530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F530..0x0048F970.
// Behavior/CFG: pushes the mutex name "IGIMUTEX" and calls CreateMutexA with
// Status/limits: STRUCTURALLY_VERIFIED; the mutex, the class registrations and
int IGI_RETAIL_CDECL Main(const igi::retail::core::RetailMainConfig* config)
{
#if defined(_WIN32)
    HANDLE const mutex = ::CreateMutexA(nullptr, TRUE, "IGI1MUTEX");
    if (mutex == nullptr)
    {
        return 1;
    }
    const DWORD last_error = ::GetLastError();
    if (last_error == ERROR_ALREADY_EXISTS || last_error == 6)
    {
        return 1;
    }
    ::CoInitialize(nullptr);

    WNDCLASSEXW window_class{};
    window_class.cbSize = sizeof(window_class);
    window_class.lpfnWndProc = reinterpret_cast<WNDPROC>(&WindowProcedure);
    window_class.hInstance = static_cast<HINSTANCE>(config->instance);
    window_class.lpszClassName = L"IGI";
    if (::RegisterClassExW(&window_class) == 0)
    {
        return 1;
    }
    g_main_window = ::CreateWindowExW(0, L"IGI", L"IGI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                       CW_USEDEFAULT, 640, 480, nullptr, nullptr,
                                       static_cast<HINSTANCE>(config->instance), nullptr);
    if (g_main_window == nullptr)
    {
        return 1;
    }
    ::ShowWindow(g_main_window, config->show_command);
    ::UpdateWindow(g_main_window);

    // The retail's per-frame loop at 0x0048F80D: the x87 precision setup,
    // the app context's isActive flag, the frame dispatch or a 500 ms sleep,
    // the loop status byte and the message pump.
    MSG message{};
    bool running = true;
    while (running)
    {
        igi::retail::core::FP_InitExtendedPrecision();
        if (AppContext_GetIsActive() != 0)
        {
            QTask_OpenEvent();
        }
        else
        {
            ::Sleep(500);
        }
        if (Loop_StatusByte == 0U)
        {
            ::DestroyWindow(g_main_window);
        }
        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }
    return 0;
#else
    (void)config;
    return 1;
#endif
}

} // extern "C"

#undef IGI_RETAIL_CDECL
