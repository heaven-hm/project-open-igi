#include "igi/retail/core/retail_main.hpp"

#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/fpu_state.hpp"
#include "igi/retail/core/window_procedure.hpp"

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

// Native: Retail_Main; retail VA 0x0048F530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F530..0x0048F970.
// Behavior/CFG: pushes the mutex name "IGIMUTEX" and calls CreateMutexA with
//           the initial owner set. When the handle is null, or the last error
//           is ERROR_ALREADY_EXISTS (0xB7) or 6, the method leaves. It then
//           initialises COM, registers the window class with the window
//           procedure at 0x0048F970, reads "filesys.cfg" and the "Innerloop"
//           script through the natives at 0x0048F0F0 and 0x0048F100, brings
//           the app context up through the native at 0x0048F140, registers the
//           Window, WinInput, Germany and USA classes through the class
//           registration helper at 0x0048F360, and runs the per-frame loop at
//           0x0048F80D.
// Status/limits: STRUCTURALLY_VERIFIED; the mutex, the class registrations and
//           the per-frame order are the retail's. The retail's setup calls
//           for the movie, subtitle, smoke, sound and joystick systems sit
//           between the script setup and the class registrations; they are
//           outside the MVP scope and this transcription skips them, and the
//           class registration helper at 0x0048F360 is not transcribed yet, so
//           the port registers no class of its own.
int IGI_RETAIL_CDECL Retail_Main(const igi::retail::core::RetailMainConfig* config)
{
#if defined(_WIN32)
    HANDLE const mutex = ::CreateMutexA(nullptr, TRUE, "IGIMUTEX");
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
    window_class.lpfnWndProc = reinterpret_cast<WNDPROC>(&Retail_WindowProcedure);
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
        if (DAT_005C8DE8 == 0U)
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
