#pragma once

// Native: Debug_Error; retail VA 0x004AF7B0. The catalog name for this address is Debug_Error
// and this is the only candidate symbol bound to it; the former Error_Show name was port
// scaffolding for the same retail body and was removed.
// Artifact: retail igi.exe, SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01,
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
//   1,384,448 bytes, image base 0x00400000, entry 0x004A50F5, machine i386.
// Tools/evidence (single decompiler: Binary Ninja, plus Radare2 CFG and angr CFGFast):
//   * Binary Ninja: function 0x004AF7B0, 89 bytes (0x004AF7B0-0x004AF808), 5 basic blocks,
//     5 edges, 29 instructions, cdecl. Basic blocks: 0x004AF7B0, 0x004AF7B9, 0x004AF7FD,
//     0x004AF806, 0x004AF808. Exact bytes:
//     a16862930085c074445768b86d5400684046a800e8ea5bffff8b4c24108d4424145051bf4046a800
//     83c9ff33c0f2aef7d14981c14046a80051e8145fffff684046a800ff156862930083c4185f
//     a16c62930085c07402ffe0c3
//   * Radare2 6.2.0 `afij`/`pdf`: size 89, 5 blocks, 5 edges, 29 instructions,
//     calltype cdecl, noreturn=false (the entry itself has a return path).
//   * angr 10.0.4.post1 CFGFast: exact entry recovered; with the retail error-initializer
//     callback model (see below) 10 blocks / 11 edges and no unresolved jumps; the entry is
//     reported returning because the no-callback path ends in `retn`.
// Signature: x86 cdecl `std::int32_t Debug_Error(const char* format, ...)`; the first stack
//   argument at [esp+4] is a printf-style format string, the remaining stack arguments are its
//   substitutions. All 274 retail call sites clean the stack (`add esp,N`), which confirms cdecl.
// Behavior: if the display callback global 0x00936268 is non-null, `sprintf` writes the literal
//   `"Fatal error:\n\n"` to the global buffer 0x00A84640, a `repne scasb` scan finds the NUL,
//   `String_VFormat` (0x004A5702) appends the formatted message at that offset, and the display
//   callback is called with 0x00A84640. The function then reads the exit callback global
//   0x0093626C: if it is null it returns EAX=0 (`retn` at 0x004AF808); otherwise it tail-dispatches
//   through it (`jmp eax` at 0x004AF806), so the callback's result becomes this function's result.
//   The function itself therefore has a genuine return path, but the initialized game never
//   reaches it: 0x00498900 (called through 0x00498950 from 0x00532405) installs
//   g_936268=0x004988D0 and g_93626C=0x00498930, and 0x00498930 ends in the CRT exit path
//   (0x004A5AD5 -> 0x004A5AF7) that calls ExitProcess(1) and does not return. The fatal call sites examined
//   here also append a self-loop after the call (`call Debug_Error; add esp,N; jmp $`), for example
//   0x005144F7/0x00514504, 0x005146F1/0x005146F9, 0x004D2425/0x004D242D, 0x004D2434/0x004D243C,
//   0x00415B65/0x00415B6D and 0x0044DFE8/0x0044DFF0, so the fatal call sequence never continues.
//   There are 274 call sites.
// Message box / logging path: the installed display callback 0x004988D0 is
//   `ShowCursor(1); MessageBoxA(g_5c8bc4, buffer, g_5c8bd4, MB_ICONHAND); ShowCursor(0);` - one
//   modal message box; retail performs no file or console logging in this path.
// Termination path: the installed exit callback 0x00498930 is
//   `if (g_5c8bf8) DebugBreak(); exit(1);` where exit is 0x004A5AD5 -> 0x004A5AF7; that CRT exit
//   runs the registered atexit callbacks and calls ExitProcess(1) (or TerminateProcess with code 1
//   when shutdown has already begun). The callbacks are both zero in the file image, so before the
//   installer runs the raw entry returns 0 without reporting anything.
// Scope/status: this header declares the retail-named C symbol only. The callback storage and the
//   global message buffer live in igi/retail/error/error_show.hpp and error_show.cpp (that unit
//   hosts the shared error state and String_VFormat; the fatal dispatcher itself is defined here),
//   so the port keeps one copy of 0x00936268/0x0093626C/0x00A84640.
//   ANALYZED and STRUCTURALLY_VERIFIED against the bytes above; BUILD_VERIFIED for MinGW x86.
//   No runtime observation was made in this task.
// Fatal-call contract: the candidate entry reproduces the retail body, including the genuine
//   return path: it returns 0 when no exit callback is installed and otherwise tail-dispatches
//   through the callback. Retail's installed exit callback never returns, and the examined
//   callers append `jmp $`, so a fatal call sequence stays terminal in the initialized game
//   without moving a self-loop into this function.
extern "C" int Debug_Error(const char* format, ...);
