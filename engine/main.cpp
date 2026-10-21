// Retail WinMain shell: window identity transcribed from igi.exe main (0x0048F530).
// Class name and title are both the retail string "Innerloop" (0x0053B534),
// registered with RegisterClassA and created with CreateWindowExA. The class
// procedure is retail 0x0048F970. Diagnostic logging is not retail behaviour.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "igi/retail/core/app_startup_natives.hpp"
#include "igi/retail/core/native_0048f970.hpp"

// Retail 0x0048F970: window procedure registered by main.
static LRESULT CALLBACK retail_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, message, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR command_line, int show)
{
    WNDCLASSA window_class{};
    window_class.style = 3;                                    // retail main 0x0048F596
    window_class.lpfnWndProc = retail_window_proc;             // retail 0x0048F970
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = instance;
    window_class.hIcon = LoadIconA(instance, MAKEINTRESOURCEA(0x7F00));   // retail 0x0048F58C
    window_class.hCursor = LoadCursorA(nullptr, MAKEINTRESOURCEA(0x7F00)); // retail 0x0048F5B8
    window_class.hbrBackground = nullptr;
    window_class.lpszMenuName = "Innerloop";                   // retail 0x00541D68 -> 0x0053B534
    window_class.lpszClassName = "Innerloop";                  // retail 0x0053B534
    if (!RegisterClassA(&window_class)) return 1;              // retail 0x0048F5E6

    // Retail main 0x0048F5F4 / 0x0048F5FA: store the command line, then the instance handle.
    StoreCommandLine(command_line);
    StoreInstanceHandle(instance);

    // Retail CreateWindowExA at 0x0048F770: class and title are both "Innerloop".
    HWND window = CreateWindowExA(0, "Innerloop", "Innerloop",
        WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    ShowWindow(window, show);
    MSG message{};
    while (GetMessageA(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return static_cast<int>(message.wParam);
}
