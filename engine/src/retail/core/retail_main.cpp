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
// Tools/evidence: Binary Ninja MCP, Radare2 and angr CFGFast on retail IGI.EXE; mutex VA 0x0048F530..0x0048F578, duplicate path 0x0048F8F0..0x0048F912.
// Behavior/CFG: CreateMutexA(NULL, TRUE, "IGIMUTEX") at 0x0048F53A; a NULL handle or GetLastError ERROR_ALREADY_EXISTS (0xB7) / ERROR_INVALID_HANDLE (6) reaches MessageBoxA(NULL, "IGI is already running.", "Error", MB_ICONERROR) and returns 0; the success path calls ReleaseMutex at 0x0048F8CA.
// Status/limits: STRUCTURALLY_VERIFIED; mutex creation and duplicate-instance path checked against retail disassembly, the 26-block Binary Ninja/Radare2 CFG and angr CFGFast; the Win32 window and per-frame loop remain partially transcribed.
int IGI_RETAIL_CDECL Main(const igi::retail::core::RetailMainConfig* config)
{
#if defined(_WIN32)
    // Retail 0x0048F544: CreateMutexA(NULL, TRUE, "IGIMUTEX"). A NULL handle
    // jumps straight to the shared failure box; otherwise GetLastError is
    // sampled for ERROR_ALREADY_EXISTS (0xB7) and, if that misses, sampled
    // again for ERROR_INVALID_HANDLE (6). All three reach the box at
    // 0x0048F8F0 and the routine returns 0.
    HANDLE const mutex = ::CreateMutexA(nullptr, TRUE, "IGIMUTEX");
    bool refuse_second_instance = mutex == nullptr;
    if (!refuse_second_instance && ::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        refuse_second_instance = true;
    }
    if (!refuse_second_instance && ::GetLastError() == ERROR_INVALID_HANDLE)
    {
        refuse_second_instance = true;
    }
    if (refuse_second_instance)
    {
        ::MessageBoxA(nullptr, "IGI is already running.", "Error", MB_ICONERROR);
        return 0;
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
    // Retail releases the mutex on the normal teardown path at 0x0048F8CA; its
    // failure paths branch past that call and never release.
    ::ReleaseMutex(mutex);
    return 0;
#else
    (void)config;
    return 1;
#endif
}

} // extern "C"

#undef IGI_RETAIL_CDECL
