#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

namespace igi::retail::error {

// Native: String_VFormat() - VA 0x004A5702 - complete port (CRT stream vsprintf wrapper).
// Behavior: initializes a stack CRT _iobuf stream over `buffer` with count `0x7FFFFFFF` and
//           flags `0x42` (_IOWRT | _IOSTRG), invokes the formatting engine `0x004A7720`
//           (`_output`) with `format` and `args`, decrements `_cnt` and either
//           flushes via `0x004A760B` (`_flsbuf`) when negative or NUL-terminates
//           `*ptr = 0`, and returns the written character count.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004A5702-0x004A5753, 81 bytes, digest dfd43dec4ebf396e9deadda3a0e5ef20a4952db80910e284b9b29f1aaa78d203;
//                 CLEAN/string/004A5702-String_VFormat.c.
// CFG: radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100% structural agreement.
// Confidence: high for exact unbounded CRT vsprintf semantics and NUL termination.
// Signature: x86 cdecl int32_t(char* buffer, const char* format, va_list args); ported with matching parameters.
// Usage/tests: called by Error_Show (0x004AF7B0); exercised by error_show_test.cpp.
std::int32_t String_VFormat(char* buffer, const char* format, std::va_list args);

// Native: sprintf() - VA 0x004A53B3 - complete port (CRT stream sprintf wrapper).
// Behavior: initializes a stack CRT _iobuf stream over `buffer` with count `0x7FFFFFFF` and
//           flags `0x42` (_IOWRT | _IOSTRG), passes `&arg_10h` (variadic argument list) to
//           `0x004A7720` (`_output`), NUL-terminates `*ptr = 0`, and returns
//           the formatted length.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004A53B3-0x004A5405, 82 bytes,
//                 digest 903e5756370f8719b677989dccf6159c0e5d23a86e6b57b11d9e7669453b7042;
//                 CLEAN/game/004A53B3-sprintf.c.
// CFG: radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100% structural agreement.
// Confidence: high for exact variadic sprintf behavior used across retail path builders and Error_Show.
// Signature: x86 cdecl int32_t(char* buffer, const char* format, ...); ported with matching parameters.
// Usage/tests: called by Error_Show (0x004AF7B0), DynObj_LocalModelsSMef (0x004CF390), and DynObj_LoadModelTexturePackage (0x004CF080); exercised by error_show_test.cpp.
std::int32_t sprintf(char* buffer, const char* format, ...);

// Native: Error_Show() - VA 0x004AF7B0 - complete port.
// Behavior: if callback `g_936268` is non-null, formats `"Fatal error:\n\n"` into global buffer
//           `0x00A84640` via sprintf (0x004A53B3), scans `0x00A84640` with `repne scasb`
//           (`i = 0xFFFFFFFF`) to find its NUL terminator (`~i - 1`), formats `format` and variadic
//           arguments at `0x00A84640 + (~i - 1)` via String_VFormat (0x004A5702), and calls
//           `g_936268(0x00A84640)`. Then reads callback `g_93626c`; if zero returns 0, otherwise
//           tail-calls `g_93626c()`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AF7B0-0x004AF809, 89 bytes,
//                 digest 1c20410541c863f90a17d98f1088afa496d65e0a6a62a8f4447fbd46ac37d72e;
//                 CLEAN/error/004AF7B0-Error_Show.c.
// CFG: radare2 5 blocks/5 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Confidence: high for exact branch, `repne scasb` length arithmetic, buffer append, and callback tailcall.
// Signature: x86 cdecl int32_t(const char* format, ...); ported with matching parameters.
// Usage/tests: retail fatal-error formatter/dispatcher; exercised by error_show_test.cpp.
using ErrorDisplayFn = void (*)(const char* text);
using ErrorExitFn = std::int32_t (*)();

std::int32_t Error_Show(const char* format, ...);

// Port-owned storage for retail globals 0x00936268, 0x0093626C, and 0x00A84640.
struct ErrorGlobalsState {
    ErrorDisplayFn g_936268 = nullptr;
    ErrorExitFn g_93626c = nullptr;
    char g_a84640[0x1000]{};
};

[[nodiscard]] ErrorGlobalsState& Error_GetGlobals();
void Error_ResetGlobalsForTest();

} // namespace igi::retail::error
