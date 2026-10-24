// Two retail start-up setters main (0x0048F530) calls between 0x0048F60E and
// 0x0048F62F, reconstructed from igi.exe (SHA-256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01).
//
// Static evidence: Binary Ninja MCP (selected decompiler: function info, pseudo C,
// disassembly), Radare2 CFG and angr CFGFast in
// build/audit/p05b-20261023T222309Z/bn/ and the P05c/D packets. Retail runtime
// evidence: Tester individual full-game Frida records under
// build/tester/20261023T220230Z/ (SetAppTitle observed from main 0x48F60E;
// SetDebugCompileState observed from main 0x48F629).
//
// The BN catalog aliases (FX_ExplosionSpawn_23 / FX_FireBallUpdate_24) are wrong
// for both addresses; the record names below are evidence-derived.

#include <cstdint>

#if defined(_WIN32)
#include <windows.h>
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C"
{
    // Port owners of the retail globals these setters bind to.
    extern char const* AppTitle;   // retail 0x005C8BD4, read by GetAppTitle 0x0048F0B0
    extern void* g_window_handle;  // retail 0x005C8BC4, written by StoreWindowHandle 0x0048F0E0
}

extern "C"
{
    // Port-owned storage for retail 0x005C8BEC. The QVM native loader currently
    // keeps a private anonymous-namespace copy of the same retail address
    // (qvm_native_loader.cpp g_5c8bec); unifying the two owners is a recorded
    // wiring task, so this definition must not be read as the single writer yet.
    std::int32_t g_5c8bec = 0;
}

extern "C"
{

// Native: SetAppTitle; retail VA 0x0048F100
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05b-20261023T222309Z/ and build/tester/20261023T220230Z/static/0x0048F100-static.json; BN catalog alias FX_ExplosionSpawn_23 is wrong
// Behavior/CFG: `ecx=[0x5C8BC4]; eax=[esp+4]; mov [0x5C8BD4],eax; if (ecx) SetWindowTextA(ecx,eax); ret` - one block; stores the title pointer in the document-title global and, when the window handle is non-null, forwards the title to SetWindowTextA; returns the argument when there is no window and the SetWindowTextA result otherwise; main calls it with "Innerloop" (0x53B534) at 0x48F60E and QTask_CreateTask calls it with "IGI" (0x536144)
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed from main 0x48F60E and from QTask_CreateTask; candidate runtime unobserved until main wiring
std::int32_t NATIVE_DECL SetAppTitle(char const* title)
{
    AppTitle = title;
#if defined(_WIN32)
    if (g_window_handle != nullptr)
    {
        return static_cast<std::int32_t>(::SetWindowTextA(
            static_cast<HWND>(g_window_handle), title));
    }
#endif
    return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(title));
}

// Native: SetDebugCompileState; retail VA 0x0048F1C0
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/audit/p05b-20261023T222309Z/ and build/tester/20261023T220230Z/static/0x0048F1C0-static.json; BN catalog alias FX_FireBallUpdate_24 is wrong
// Behavior/CFG: `movsx eax,byte [esp+4]; mov [0x5C8BEC],eax; ret` - one block: sign-extends the low byte of the argument into the debug-compile flag and returns the sign-extended value; main calls it with 1 at 0x48F629
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (main 0x48F629, arg 1); candidate runtime unobserved until main wiring; the flag global is currently also written by the QVM native loader, which is a port binding to confirm during wiring
std::int32_t NATIVE_DECL SetDebugCompileState(std::int32_t value)
{
    const std::int32_t narrowed = static_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    g_5c8bec = narrowed;
    return narrowed;
}

} // extern "C"
