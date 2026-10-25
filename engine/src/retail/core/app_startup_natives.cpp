#include "dev_log.hpp"
#include "igi/retail/core/app_startup_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

char const* CommandLine = nullptr;
void* InstanceHandle = nullptr;
char const* AppTitle = nullptr;

// Native: StoreCommandLine; retail VA 0x0048F0C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 6.0.10601, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; VA 0x0048F0C0..0x0048F0CA (10 bytes, 3 instructions, 1 basic block, no edges).
// Behavior/CFG: bytes 8b442404 a3c88b5c00 c3: mov eax,[esp+4]; mov [0x005C8BC8],eax; ret. One block, no calls: the cdecl argument (main's lpCmdLine, pushed at 0x0048F5F3) is written to the 32-bit global at 0x005C8BC8 and the identical bits are returned in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the retail load/store/return are reproduced. Paired start-up runtime comparison and the live behavioral runner are still required for acceptance.
char const* NATIVE_DECL StoreCommandLine(char const* command_line)
{
    NATIVE_LOG_ENTER("StoreCommandLine", "0x0048F0C0");
    CommandLine = command_line;
    return command_line;
}

// Native: GetCommandLine; retail VA 0x0048F080.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 6.0.10601 free (UI MCP 0.1.0), Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; VA 0x0048F080..0x0048F086 (6 bytes, bytes a1c88b5c00c3, 2 instructions, 1 basic block, no edges).
// Behavior/CFG: mov eax,[0x005C8BC8]; ret. One block, no calls and no arguments: returns the 32-bit command-line pointer that 0x0048F0C0 stores at 0x0048F0C4. The only direct caller is 0x0048F360 at 0x0048F369, which scans that string for its terminator and parses it with the " \t" separators; the global itself is written only at 0x0048F0C4. The source identifier GameCommandLine avoids the Win32 GetCommandLine macro while naming the same game command-line pointer.
// Status/limits: STRUCTURALLY_VERIFIED; the retail load and EAX return are reproduced by a candidate that reads the same candidate global its own StoreCommandLine writes. Paired start-up runtime comparison is still required for acceptance.
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
char const* NATIVE_DECL GameCommandLine()
{
    NATIVE_LOG_ENTER("GameCommandLine", "0x0048F0C0");
    return CommandLine;
}

// Native: StoreInstanceHandle; retail VA 0x0048F0D0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; VA 0x0048F0D0..0x0048F0D9 (10 bytes, bytes 8b442404 a3c08b5c00 c3, 3 instructions, 1 basic block, no edges).
// Behavior/CFG: mov eax,[esp+4]; mov [0x005C8BC0],eax; ret. One cdecl argument; the argument bits are stored at the 32-bit global 0x005C8BC0 and returned unchanged in EAX. The only caller is main 0x0048F530 at 0x0048F5FA, which does not read EAX afterwards.
// Status/limits: STRUCTURALLY_VERIFIED; retail load/store/return reproduced, the translation unit compiles for x86, and the 10-case retail-vs-candidate differential passes. Paired startup runtime comparison is still required for acceptance.
void* NATIVE_DECL StoreInstanceHandle(void* instance)
{
    NATIVE_LOG_ENTER("StoreInstanceHandle", "0x0048F0C0");
    InstanceHandle = instance;
    return instance;
}

// Native: GetAppTitle; retail VA 0x0048F0B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 6.0.10601 free (UI MCP 0.1.0), Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; VA 0x0048F0B0..0x0048F0B6 (6 bytes, bytes a1d48b5c00c3, 2 instructions, 1 basic block, no edges).
// Behavior/CFG: mov eax,[0x005C8BD4]; ret. One block, no calls and no arguments: returns the 32-bit pointer that 0x0048F100 stores at 0x0048F10C before passing it to SetWindowTextA (the start-up call at 0x0048F60E supplies "Innerloop"); the only direct caller is 0x4988D0 at 0x4988DD, which passes the value to MessageBoxA as lpCaption. This is not the instance handle: 0x005C8BC0 is written by 0x0048F0D0 and read by the separate getter 0x0048F090.
// Status/limits: STRUCTURALLY_VERIFIED; the candidate now reads a distinct 4-byte global for retail 0x005C8BD4 instead of returning the instance handle. The writer is now linked as SetAppTitle (engine/src/retail/core/app_context_extra.cpp), which also forwards the title to SetWindowTextA when the window handle exists; the candidate does not call it yet because the retail main order is still being wired, so the value remains null until then; paired runtime comparison is still required.
char const* NATIVE_DECL GetAppTitle()
{
    NATIVE_LOG_ENTER("GetAppTitle", "0x0048F0C0");
    return AppTitle;
}

} // extern "C"

#undef NATIVE_DECL
