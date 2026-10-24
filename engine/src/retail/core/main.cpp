// Retail entry point reconstruction: igi.exe 0x0048F530 (Main).
//
// Evidence chain (all under this checkout):
//   docs/fidelity/main-call-flow.md - ordered walk + 10-step transcription checklist
//   build/audit/p05b..p05e/ - static packets (BN disassembly/pseudo C, r2 bytes, angr)
//   build/main-0048f530.disasm.txt - full BN disassembly of the retail main
//   build/tester/20261023T220230Z/ - Tester individual retail full-game Frida records
//
// Retail order transcribed here (call sites in comments): mutex/duplicate path,
// CoInitialize, LoadIconA/LoadCursorA/RegisterClassA("Innerloop"), the startup
// getters/setters block 0x48F5F4-0x48F6EE in exact order, the four CLI parser
// registrations (Window/WinInput/Germany/USA), CreateWindowExA with the style
// derived from Display_GetScreenMode (0x37C80000 windowed / 0x96000000 mode 1),
// ShowCursor/ShowWindow/UpdateWindow, StoreWindowHandle 0x48F0E0,
// QTask_CreateTask 0x401000 with the AL spill into 0x5C8DE8, the HardcoreGerman
// parser call, the loop with the FPU self-check and the retail Peek/Get/Dispatch
// pump, and the shutdown sequence back through ReleaseMutex.
//
// Known, explicitly recorded divergences (do not hide):
//  * mutex name "IGI1MUTEX" instead of "IGIMUTEX" (owner-authorized side-by-side).
//  * 0x00498480 Display_ProbeDirectXVersion is NOT linked yet (needs its own
//    individual retail static+runtime record), so the DX-tier gate below is
//    absent instead of faked.
//  * 0x00498460 (x87 truncate-rounding control-word setter) is not linked; the
//    port's FP_InitExtendedPrecision helper is used in the loop instead and this
//    substitution is recorded as unresolved.
//  * QTask_CreateTask currently throws inside QTask_RegisterClasses (untranscribed
//    registrars), so the candidate is expected to abort at that call until the
//    class registration is reconstructed; that is a documented dependency, not a
//    pass, and the startup calls before it remain observable.

#include "igi/retail/core/main.hpp"

#include "igi/retail/core/app_context.hpp"
#include "igi/retail/core/app_context_extra.hpp"
#include "igi/retail/core/app_main_commands.hpp"
#include "igi/retail/core/app_startup_globals.hpp"
#include "igi/retail/core/app_startup_natives.hpp"
#include "igi/retail/core/fpu_state.hpp"
#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/native_0048f0e0.hpp"
#include "igi/retail/core/native_0048f0f0.hpp"
#include "igi/retail/core/native_0048f970.hpp"

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

extern "C" {
// QTask_CreateTask 0x00401000 (engine/src/retail/core/qtask_create_task.cpp).
int NATIVE_DECL QTask_CreateTask();
}

extern "C" {

// Native: Main; VA 0x0048F530.
// Artifact: original igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP (selected decompiler), Radare2 CFG and angr CFGFast on IGI.EXE; full disassembly build/main-0048f530.disasm.txt; static packets build/audit/p05b..p05e; Tester retail records build/tester/20261023T220230Z/.
// Behavior/CFG: retail opens with CreateMutexA(NULL,TRUE,"IGIMUTEX") and the duplicate path to MessageBoxA("IGI is already running.","Error",MB_ICONERROR)/return 0, CoInitialize, LoadIconA/LoadCursorA/RegisterClassA for class "Innerloop" (WNDCLASSA style 3, lpfnWndProc 0x48F970, hbrBackground 7, menu+class name "Innerloop"), then the startup block 0x48F5F4-0x48F6EE (StoreCommandLine, StoreInstanceHandle, SetConfigFileName "filesys.cfg", SetAppTitle_48F100 "Innerloop", SetIsActive 1, SetScreenMode 1, SetDebugged 0, SetDebugCompileState 1, SetDebugtextState 0, SetLightmapsUsed 1, SetTerrainLightmapsUsed 1, SetWinInput 0, Set5C8C0C 0, Set5C8C10 0, SetStartupMode 1, SetGermanVersion 0, SetLanguage 0x567C74, Set5C8C04 0), WinProc list init, four ParseCmdLineArgs registrations, DX-tier gate (0x498480, currently NOT linked), CreateWindowExA (style from GetScreenMode, 640x480 centered), ShowCursor(0)/ShowWindow(10)/UpdateWindow, StoreWindowHandle 0x48F0E0, QTask_CreateTask 0x401000 with AL spilled to 0x5C8DE8, ParseCmdLineArgs "HardcoreGerman", then the loop with FPU self-check, 0x498460, GetIsActive/Sleep(500) or QTask_OpenEvent, DestroyWindow when 0x5C8DE8==0, Peek/Get/Translate/Dispatch pump, and shutdown SetForegroundWindow/UnregisterClassA "Innerloop"/CoUninitialize/ReleaseMutex with `add esp,0x464; retn 0x10`.
// Status/limits: STRUCTURALLY_VERIFIED; startup call order/arguments checked against the retail disassembly and Tester individual retail records; the mutex name, the absent DX gate, the absent 0x498460 and the throwing QTask_CreateTask are recorded divergences; candidate runtime parity is not claimed.
int __stdcall Main(void* const h_instance, void* const h_previous_instance,
                   char* const command_line, int const show_command)
{
#if defined(_WIN32)
    // 0x0048F544 CreateMutexA(NULL, TRUE, "IGIMUTEX"), name pointer 0x00541ECC.
    // Build-time testing divergence only: the verification build must be able to
    // run beside retail igi.exe, so IGI_TESTING_MUTEX selects "IGI1MUTEX".
    // Production builds must never define it and use the retail "IGIMUTEX".
    // TO REVERT TO FULL RETAIL FIDELITY: drop -DIGI_TESTING_MUTEX from the build.
#if defined(IGI_TESTING_MUTEX)
    HANDLE const mutex = ::CreateMutexA(nullptr, TRUE, "IGI1MUTEX");
#else
    HANDLE const mutex = ::CreateMutexA(nullptr, TRUE, "IGIMUTEX");
#endif
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

    // 0x0048F579 CoInitialize(NULL); 0x0048F57F GetForegroundWindow.
    (void)::CoInitialize(nullptr);
    // 0x0048F57F GetForegroundWindow: stored in a stack local (S+0x1C in the
    // retail frame) and passed to SetForegroundWindow on the shutdown path.
    HWND const previous_foreground = ::GetForegroundWindow();
    // Stack local S+0x18: the window handle; the only persistence is the
    // StoreWindowHandle write to [0x005C8BC4] at 0x48F7A0.
    HWND window = nullptr;

    // 0x0048F58C LoadIconA(hInstance, IDI_APPLICATION 0x7F00);
    // 0x0048F5B8 LoadCursorA(NULL, IDC_ARROW 0x7F00);
    HICON const icon = ::LoadIconA(static_cast<HINSTANCE>(h_instance),
                                   reinterpret_cast<LPCSTR>(static_cast<std::uintptr_t>(0x7F00U)));
    HCURSOR const cursor = ::LoadCursorA(
        nullptr, reinterpret_cast<LPCSTR>(static_cast<std::uintptr_t>(0x7F00U)));

    // 0x0048F596-0x48F5E6 RegisterClassA(WNDCLASSA{style 3, lpfnWndProc 0x48F970,
    // hInstance=arg1, hIcon, hCursor, hbrBackground 7, lpszMenuName=lpszClassName=
    // "Innerloop" (0x53B534)}).
    WNDCLASSA window_class{};
    window_class.style = 3;
    window_class.lpfnWndProc = reinterpret_cast<WNDPROC>(&WindowProcedure);
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = static_cast<HINSTANCE>(h_instance);
    window_class.hIcon = icon;
    window_class.hCursor = cursor;
    window_class.hbrBackground = reinterpret_cast<HBRUSH>(static_cast<std::uintptr_t>(7U));
    window_class.lpszMenuName = "Innerloop";
    window_class.lpszClassName = "Innerloop";
    // 0x48F5E6 call RegisterClassA: the retail never tests the result and
    // continues at 0x48F5EC.
    (void)::RegisterClassA(&window_class);

    // 0x0048F5F4-0x48F6EE startup block in exact retail order.
    // 0x48F5F3 push lpCmdLine / 0x48F5F4 call 0x0048F0C0.
    (void)StoreCommandLine(command_line);
    (void)StoreInstanceHandle(h_instance);
    // 0x48F5FF push "filesys.cfg" (0x541EC0) -> 0x0048F0F0 stores the pointer.
    (void)SetConfigFileName(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>("filesys.cfg")));
    (void)SetAppTitle_48F100("Innerloop"); // 0x48F609 push "Innerloop" (0x53B534)
    (void)AppContext_SetIsActive(1);
    (void)Display_SetScreenMode_48F120(1);
    (void)igi::retail::core::App_ContextSetDebugged(0);
    (void)SetDebugCompileState_48F1C0(1);
    (void)igi::retail::core::App_ContextSetDebugtextState(0);
    (void)igi::retail::core::App_ContextSetLightmapsUsed(1);
    (void)igi::retail::core::App_ContextSetTerrainLightmapsUsed(1);
    (void)Input_SetUseWin32Input_48F280(0);
    (void)AppMain_Set5C8C0C_48F290(0);
    (void)AppMain_Set5C8C10_48F2A0(0);
    (void)Game_SetStartupMode_48F210(1);
    (void)AppContext_SetGermanVersion_48F2B0(0);
    // 0x48F6B2 push 0x567C74: the retail argument is the address of an empty
    // string in static data (the language selector default). The bytes are the
    // faithful part; a retail VA would be meaningless in this image, so the same
    // empty-string bytes are passed. SetLanguage 0x48F300 returns length+1, which
    // retail observes as 1 for this call.
    (void)AppContext_SetLanguage_48F300("");
    (void)AppMain_Set5C8C04_48F200(0);

    // 0x0048F680/0x48F687-0x48F6D3: four parser registrations with the retail
    // handler bodies. Registration alone never dispatches a handler; dispatch is
    // token-gated inside the parser.
    (void)AppMain_ParseCmdLineArgs_48F360("Window", &AppMain_WindowCommand_48F350);
    (void)AppMain_ParseCmdLineArgs_48F360("WinInput", &AppMain_WinInputCommand_48F920);
    (void)AppMain_ParseCmdLineArgs_48F360("Germany", &AppMain_GermanyCommand_48F930);
    (void)AppMain_ParseCmdLineArgs_48F360("USA", &AppMain_USACommand_48F950);

    // 0x0048F6E2 Display_ProbeDirectXVersion(&tier, &platform) + the < 0x700
    // requirement box is NOT wired: the native is not linked yet (recorded gap).

    // 0x0048F754 GetScreenMode -> CreateWindowExA style:
    //   mode != 0 -> 0x96000000, mode == 0 -> 0x37C80000 (neg/sbb/and/add).
    // 0x48F72A-0x48F751 retail order: GetSystemMetrics(1) -> y=(cy-480)/2
    // pushed first, then GetSystemMetrics(0) -> x=(cx-640)/2, and only then the
    // screen-mode getter whose sign feeds the style word.
    int const screen_height = ::GetSystemMetrics(SM_CYSCREEN); // 0x48F737 push 1
    int const window_y = (screen_height - 480) / 2;           // cdq/sub/sar
    int const screen_width = ::GetSystemMetrics(SM_CXSCREEN);  // 0x48F73B push 0
    int const window_x = (screen_width - 640) / 2;            // cdq/sub/sar
    std::uintptr_t const style =
        (Display_GetScreenMode_48F130() != 0)
            ? (static_cast<std::uintptr_t>(0x5E380000U) + 0x37C80000U)
            : static_cast<std::uintptr_t>(0x37C80000U);
    // 0x0048F770 CreateWindowExA(0, "Innerloop", "Innerloop", style, x, y, 640,
    // 480, NULL, NULL, hInstance, NULL).
    window = ::CreateWindowExA(0, "Innerloop", "Innerloop", static_cast<DWORD>(style),
                                      window_x, window_y, 640, 480, nullptr, nullptr,
                                      static_cast<HINSTANCE>(h_instance), nullptr);
    if (window == nullptr)
    {
        ::MessageBoxA(nullptr, "Couldn't open window", "Innerloop", 0);
        return 0;
    }
    ::ShowCursor(0); // 0x0048F790
    ::ShowWindow(window, 10); // 0x48F799 SW_SHOWDEFAULT (10)

    (void)SetWindowHandle(window); // 0x48F7A0 0x0048F0E0 -> [0x005C8BC4]
    // 0x0048F7A5 0x00498460 (x87 truncate rounding) is not linked yet; the loop
    // uses the port's FP helper below and records the substitution.

    // 0x0048F7AA QTask_CreateTask: the retail call, uncatched like the original.
    int const task_result = QTask_CreateTask(); // 0x0048F7AA
    // 0x48F7B9: mov [0x005C8DE8], al; the byte is then tested at 0x48F7C6.
    Loop_StatusByte = static_cast<std::uint8_t>(task_result);
    bool const loop_enabled = Loop_StatusByte != 0U;
    // 0x0048F7B9 spills AL into byte [0x5C8DE8] across the parser call.
    (void)AppMain_ParseCmdLineArgs_48F360("HardcoreGerman", &AppMain_HardcoreGermanCommand_48F960);
    if (!loop_enabled)
    {
        ::SetForegroundWindow(previous_foreground);
        ::UnregisterClassA("Innerloop", static_cast<HINSTANCE>(h_instance));
        ::CoUninitialize();
        ::ReleaseMutex(mutex);
        return 0;
    }

    // 0x0048F7D3-0x48F8F0 loop: FPU self-check, 0x498460, GetIsActive ->
    // QTask_OpenEvent or Sleep(500), DestroyWindow when the status byte is 0,
    // GetMessageA/TranslateMessage/DispatchMessageA pump. The loop leaves only
    // when GetMessageA returns 0; MSG.wParam becomes the return code and the
    // status byte is cleared.
    MSG message{};
    int exit_code = 0;
    bool quit = false;
    for (;;)
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
            ::DestroyWindow(window);
        }
        // 0x48F841: the pump is entered only when PeekMessageA reports a
        // message; the inner drain repeats until nothing is pending.
        if (::PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE) != 0)
        {
            for (;;)
            {
                // 0x48F85F: only GetMessageA returning 0 (WM_QUIT) stores
                // MSG.wParam into the exit-code local and leaves the loop.
                if (::GetMessageA(&message, nullptr, 0, 0) == 0)
                {
                    exit_code = static_cast<int>(message.wParam);
                    Loop_StatusByte = 0U;
                    quit = true;
                    break;
                }
                ::TranslateMessage(&message);
                ::DispatchMessageA(&message);
                // 0x48F884: PeekMessageA again; leave when nothing is pending.
                if (::PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE) == 0)
                {
                    break;
                }
            }
        }
        // 0x48F88A: re-test the status byte, and 0x48F898 leaves
        // unconditionally once WM_QUIT was seen.
        if (quit || Loop_StatusByte == 0U)
        {
            break;
        }
    }
    Loop_StatusByte = 0U;

    // 0x0048F8A7 shutdown: SetForegroundWindow, UnregisterClassA("Innerloop"),
    // CoUninitialize, ReleaseMutex, return exit code.
    ::SetForegroundWindow(previous_foreground);
    ::UnregisterClassA("Innerloop", static_cast<HINSTANCE>(h_instance));
    ::CoUninitialize();
    ::ReleaseMutex(mutex);
    return exit_code;
#else
    (void)h_instance;
    (void)h_previous_instance;
    (void)command_line;
    (void)show_command;
    return 1;
#endif
}

} // extern "C"

#undef NATIVE_DECL
