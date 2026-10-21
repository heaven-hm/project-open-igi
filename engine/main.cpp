// Diagnostic Win32 shell and debugger logging; not retail game transcription.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static LRESULT CALLBACK diagnostic_window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show)
{
    OutputDebugStringW(L"OpenIGI diagnostic shell starting; no retail natives implemented.\n");
    WNDCLASSW window_class{};
    window_class.lpfnWndProc = diagnostic_window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = L"OpenIGIDiagnostic";
    if (!RegisterClassW(&window_class)) return 1;
    HWND window = CreateWindowExW(0, window_class.lpszClassName, L"OpenIGI diagnostic",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, instance, nullptr);
    if (!window) return 1;
    ShowWindow(window, show);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    OutputDebugStringW(L"OpenIGI diagnostic shell exiting.\n");
    return static_cast<int>(message.wParam);
}
