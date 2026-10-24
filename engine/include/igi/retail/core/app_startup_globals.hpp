#pragma once

// Retail startup globals written/read by the methods main (0x0048F530) calls
// during startup, reconstructed from igi.exe (SHA-256
// dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01).
//
// Evidence: Binary Ninja (selected decompiler) function info/pseudo C/
// disassembly, Radare2 CFG and angr CFGFast per address (build/native-analysis/
// 0x0048F120..0x0048F300/), the P05b static packet
// (build/audit/p05b-20261023T222309Z/per-native/), and the Tester individual
// retail full-game Frida records (build/tester/20261023T220230Z/individual/,
// retail runtime/retail-startup-globals-v2.json).
//
// Catalog names (FX_*) are wrong for this whole block; labels below come from
// the retail bytes, the call-site arguments in main, and the observed callers.
// 0x5C8C04/0x5C8C0C/0x5C8C10 have no evidenced readers, so their setters keep
// address-derived labels and no semantic claim.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Native: Input_SetUseWin32Input; retail VA 0x0048F280. Writes
// 0x005C8BE0. Callers: main 0x0048F643 with 0; the "WinInput" CLI handler with 1.
std::int32_t NATIVE_DECL Input_SetUseWin32Input(std::int32_t value);

// Native: AppMain_Set5C8C0C; retail VA 0x0048F290. Writes 0x005C8C0C.
// Caller main 0x0048F649 with 0. No reader evidenced; no semantic name claimed.
std::int32_t NATIVE_DECL AppMain_Set5C8C0C(std::int32_t value);

// Native: AppMain_Set5C8C10; retail VA 0x0048F2A0. Writes 0x005C8C10.
// Caller main 0x0048F64F with 0. No reader evidenced; no semantic name claimed.
std::int32_t NATIVE_DECL AppMain_Set5C8C10(std::int32_t value);

// Native: Game_SetStartupMode; retail VA 0x0048F210. Writes 0x005C8C08.
// Callers: main 0x0048F656 with 1; the "Level" handler (0x415B10) with 0 inside
// QTask_CreateTask -> Game_Init. 0x0048F220 reads it through QTask_CreateTask.
std::int32_t NATIVE_DECL Game_SetStartupMode(std::int32_t value);

// Native: AppContext_SetGermanVersion; retail VA 0x0048F2B0. Writes
// 0x005C8BDC. Callers: main 0x0048F65C with 0; "Germany" handler with 1;
// "HardcoreGerman" handler with 0.
std::int32_t NATIVE_DECL AppContext_SetGermanVersion(std::int32_t value);

// Native: AppContext_SetLanguage; retail VA 0x0048F300. Unbounded
// strlen+1 byte copy of the argument string into the retail buffer 0x005C8C20;
// returns the copied length including the NUL. Callers: main 0x0048F669 with
// 0x00567C74 (empty at image load), "Germany" with "Germany", "USA" with "USA".
std::int32_t NATIVE_DECL AppContext_SetLanguage(const char* language);

// Native: AppMain_Set5C8C04; retail VA 0x0048F200. Writes 0x005C8C04.
// Caller main 0x0048F66F with 0. No reader evidenced; no semantic name claimed.
std::int32_t NATIVE_DECL AppMain_Set5C8C04(std::int32_t value);

// Native: Display_SetScreenMode; retail VA 0x0048F120. Sign-extends the
// low byte of the argument and stores it in 0x005C8C00, returning the
// sign-extended value. Callers in `window level1`: main 0x0048F61C with 1, then
// the "Window" CLI handler (0x48F350) with 0 at 0x0048F352.
std::int32_t NATIVE_DECL Display_SetScreenMode(std::int32_t mode);

// Native: Display_GetScreenMode; retail VA 0x0048F130. Returns
// 0x005C8C00. Observed 64 calls in the `window level1` startup window: one from
// main 0x0048F754 (after the Window handler reset the mode to 0) and later
// display/menu callers.
std::int32_t NATIVE_DECL Display_GetScreenMode();

// Retail storage the reconstructed setters write. The port owns these globals
// here; 0x005C8C08 is deliberately NOT defined here because the existing
// AppContext_StartupMode (qtask_create_task.cpp) already owns that address, and
// 0x005C8C00 is owned by the SetScreenMode/GetScreenMode pair below.
extern std::int32_t g_5c8be0;  // 0x005C8BE0
extern std::int32_t g_5c8c04;  // 0x005C8C04
extern std::int32_t g_5c8c0c;  // 0x005C8C0C
extern std::int32_t g_5c8c10;  // 0x005C8C10
extern std::int32_t g_5c8bdc;  // 0x005C8BDC
extern std::int32_t g_5c8c00;  // 0x005C8C00
extern char g_5c8c20[];        // 0x005C8C20 language buffer (see .cpp limitation)

} // extern "C"
