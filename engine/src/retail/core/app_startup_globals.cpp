// Retail startup-global natives main (0x0048F530) calls between 0x0048F61C and
// 0x0048F674, reconstructed byte-exactly from igi.exe.
//
// Static evidence: Binary Ninja MCP (selected decompiler: function info, pseudo C,
// disassembly), Radare2 CFG and angr CFGFast per address in
// build/native-analysis/0x0048F120|capture.json .. 0x0048F300|capture.json, and
// the P05b packet build/audit/p05b-20261023T222309Z/per-native/.
// Retail runtime evidence (Tester, suspended spawn, full game `igi.exe window
// level1`, cwd /Users/haseeb-mir/Downloads/IGI): build/tester/20261023T220230Z/
// runtime/retail-startup-globals-v2.json with per-native records under
// build/tester/20261023T220230Z/individual/. Observed call order and arguments:
// 0x48F120(1) main; 0x48F280(0); 0x48F290(0); 0x48F2A0(0); 0x48F210(1) main;
// 0x48F2B0(0); 0x48F300(0x00567C74); 0x48F200(0); then the "Window" CLI handler
// 0x48F350 calls 0x48F120(0) at 0x48F352, and main reads 0x48F130 at 0x48F754
// (observed EAX=0, so the window style path takes the 0x37C80000 branch).
//
// Catalog names (FX_*) are wrong for this block; labels are evidence-derived.
// The candidate does not call these yet (retail main order wiring is P13), so
// no candidate-runtime observation exists and no status above
// STRUCTURALLY_VERIFIED is claimed.

#include "dev_log.hpp"
#include "igi/retail/core/app_startup_globals.hpp"

#include <cstddef>
#include <cstdint>

extern "C"
{
    // The existing owner of retail 0x005C8C08 (engine/src/retail/core/qtask_create_task.cpp).
    extern std::int32_t AppContext_StartupMode;
}

extern "C"
{
    std::int32_t g_5c8be0 = 0; // 0x005C8BE0
    std::int32_t g_5c8c04 = 0; // 0x005C8C04
    std::int32_t g_5c8c0c = 0; // 0x005C8C0C
    std::int32_t g_5c8c10 = 0; // 0x005C8C10
    std::int32_t g_5c8bdc = 0; // 0x005C8BDC
    std::int32_t g_5c8c00 = 0; // 0x005C8C00
    // Retail 0x005C8C20. The image only evidences the start address (the
    // buffer extent is not established by any reference); the observed inputs
    // are "" / "Germany" / "USA". The copy below keeps the retail's unbounded
    // strlen+1 semantics; the extent limitation is recorded in the manifest.
    char g_5c8c20[0x40] = {};
}

extern "C"
{

// Native: Display_SetScreenMode; retail VA 0x0048F120
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F120/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F120.json
// Behavior/CFG: one block `movsx eax,byte [esp+4]; mov [0x5C8C00],eax; retn` (bytes 0f be 44 24 04 a3 00 8c 5c 00 c3); stores the sign-extended low byte, returns it; observed calls: main 0x0048F61C with 1, Window CLI handler 0x48F352 with 0
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed (2 calls, values 1 then 0); candidate runtime unobserved until the retail main/handler order is wired
std::int32_t NATIVE_DECL Display_SetScreenMode(std::int32_t mode)
{
    NATIVE_LOG_ENTER("Display_SetScreenMode", "0x0048F120");
    const std::int32_t value = static_cast<std::int8_t>(static_cast<std::uint8_t>(mode));
    g_5c8c00 = value;
    return value;
}

// Native: Display_GetScreenMode; retail VA 0x0048F130
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F130/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F130.json
// Behavior/CFG: one block `mov eax,dword [0x5C8C00]; retn` (bytes a1 00 8c 5c 00 c3); returns the current screen-mode word; main calls it at 0x0048F754 and the observed EAX is 0 after the Window handler, giving the 0x37C80000 CreateWindowExA style branch; 64 calls observed in the window (1 from main, 63 later display/menu callers)
// Status/limits: STRUCTURALLY_VERIFIED; retail full-game runtime observed; candidate runtime unobserved until the retail main/handler order is wired
std::int32_t NATIVE_DECL Display_GetScreenMode()
{
    NATIVE_LOG_ENTER("Display_GetScreenMode", "0x0048F120");
    return g_5c8c00;
}

// Native: Input_SetUseWin32Input; retail VA 0x0048F280
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F280/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F280.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8BE0],eax; retn` (bytes 8b 44 24 04 a3 e0 8b 5c 00 c3); returns the argument; main calls with 0 at 0x48F643, the "WinInput" CLI handler with 1 (input-path flag; name proposed from that caller)
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call, arg 0, store 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL Input_SetUseWin32Input(std::int32_t value)
{
    NATIVE_LOG_ENTER("Input_SetUseWin32Input", "0x0048F120");
    g_5c8be0 = value;
    return value;
}

// Native: AppMain_Set5C8C0C; retail VA 0x0048F290
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F290/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F290.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8C0C],eax; retn`; returns the argument; main calls at 0x48F649 with 0; no reader of 0x5C8C0C was found, so the label stays address-derived
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call, arg 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL AppMain_Set5C8C0C(std::int32_t value)
{
    NATIVE_LOG_ENTER("AppMain_Set5C8C0C", "0x0048F120");
    g_5c8c0c = value;
    return value;
}

// Native: AppMain_Set5C8C10; retail VA 0x0048F2A0
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F2A0/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F2A0.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8C10],eax; retn`; returns the argument; main calls at 0x48F64F with 0; no reader evidenced, address-derived label
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call, arg 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL AppMain_Set5C8C10(std::int32_t value)
{
    NATIVE_LOG_ENTER("AppMain_Set5C8C10", "0x0048F120");
    g_5c8c10 = value;
    return value;
}

// Native: Game_SetStartupMode; retail VA 0x0048F210
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F210/capture.json; P05b per-native packet; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F210.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8C08],eax; retn`; returns the argument; main calls with 1 at 0x48F656, then the "Level" handler 0x415B10 calls with 0 at 0x415B26 inside Game_Init; QTask_CreateTask reads it through 0x0048F220 and takes Flow_SetState(4) on 0; observed two calls in order (1, 0)
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (2 calls, args 1 then 0, final word 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL Game_SetStartupMode(std::int32_t value)
{
    NATIVE_LOG_ENTER("Game_SetStartupMode", "0x0048F120");
    AppContext_StartupMode = value;
    return value;
}

// Native: AppContext_SetGermanVersion; retail VA 0x0048F2B0
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F2B0/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F2B0.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8BDC],eax; retn`; returns the argument; main calls with 0 at 0x48F65C; the "Germany" CLI handler calls with 1, "HardcoreGerman" with 0 (German-version flag; name proposed from those callers)
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call, arg 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL AppContext_SetGermanVersion(std::int32_t value)
{
    NATIVE_LOG_ENTER("AppContext_SetGermanVersion", "0x0048F120");
    g_5c8bdc = value;
    return value;
}

// Native: AppContext_SetLanguage; retail VA 0x0048F300
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F300/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F300.json
// Behavior/CFG: single 41-byte block: `edi=[esp+4]; ecx=-1; eax=0; repne scasb; not ecx; sub edi,ecx; eax=ecx; esi=edi; edi=[0x5C8C20]; shr ecx,2; rep movsd; ecx=eax; and ecx,3; rep movsb` - unbounded strlen+1 byte copy of the argument string into 0x005C8C20 (dword body plus byte tail), returns the copied length including the NUL; main calls with the empty image buffer 0x00567C74 (return 1), the German/USA handlers pass their language names
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call in this scenario, arg 0x00567C74, EAX=1, NUL stored); candidate runtime unobserved until wiring; the retail buffer extent is not evidenced by any reference (port buffer is 0x40 bytes, unbounded retail copy preserved for the evidenced inputs)
std::int32_t NATIVE_DECL AppContext_SetLanguage(const char* language)
{
    NATIVE_LOG_ENTER("AppContext_SetLanguage", "0x0048F120");
    std::int32_t length_with_nul = 0;
    while (language[length_with_nul] != '\0')
    {
        ++length_with_nul;
    }
    ++length_with_nul;
    for (std::int32_t index = 0; index < length_with_nul; ++index)
    {
        g_5c8c20[index] = language[index];
    }
    return length_with_nul;
}

// Native: AppMain_Set5C8C04; retail VA 0x0048F200
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x0048F200/capture.json; Tester retail record build/tester/20261023T220230Z/handoff/0x0048F200.json
// Behavior/CFG: one block `mov eax,[esp+4]; mov [0x5C8C04],eax; retn`; returns the argument; main calls at 0x48F66F with 0; no reader evidenced, address-derived label
// Status/limits: STRUCTURALLY_VERIFIED; retail runtime observed (1 call, arg 0); candidate runtime unobserved until wiring
std::int32_t NATIVE_DECL AppMain_Set5C8C04(std::int32_t value)
{
    NATIVE_LOG_ENTER("AppMain_Set5C8C04", "0x0048F120");
    g_5c8c04 = value;
    return value;
}

} // extern "C"
