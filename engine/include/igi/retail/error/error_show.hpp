#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdio>

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
// Usage: called by Debug_Error (0x004AF7B0) to append the formatted fatal message.
std::int32_t String_VFormat(char* buffer, const char* format, std::va_list args);

// Native: sprintf() - VA 0x004A53B3 - CRT-visible function, no port-owned body.
// Retail: the statically linked MSVC CRT sprintf. 0x004A53B3-0x004A5405 (82 bytes), x86 cdecl
//         variadic `int sprintf(char* buffer, const char* format, ...)`: builds a stack _iobuf over
//         `buffer` (counter 0x7FFFFFFF, flags 0x42 = _IOWRT|_IOSTRG), passes `&arg_10h` (the
//         variadic list) to `_output` (0x004A7720), NUL-terminates `*ptr` unless `_output` drove
//         the counter negative, flushes via `_flsbuf` (0x004A760B) in that case, and returns the
//         formatting result. Binary Ninja reports 4 blocks and 484 callsites across the game;
//         radare2 recovers the same 82-byte, 4-block/4-edge graph; angr CFGFast recovers the same
//         entry with no unresolved jumps.
// Port: retail calls the CRT entry directly, so the port has no wrapper and no second symbol.
//       `using ::sprintf` binds `igi::retail::error::sprintf` to the toolchain's CRT-visible
//       sprintf (mingw-w64/UCRT in the built candidate); the candidate therefore exposes exactly
//       one `sprintf` symbol, the same CRT entry its retail callsites use. The candidate does not
//       reproduce retail's _iobuf/_output instruction sequence; that limitation is recorded in
//       the evidence.
// Evidence: docs/fidelity/evidence/0x004A53B3-{binaryninja,radare2,angr-20261023,semantic-review,build}.json.
using ::sprintf;

// Port-owned storage for retail globals 0x00936268 (display callback), 0x0093626C (exit
// callback) and 0x00A84640 (message buffer). The fatal dispatcher that consumes them is
// Debug_Error (0x004AF7B0), defined in engine/src/retail/core/retail_error.cpp.
using ErrorDisplayFn = void (*)(const char* text);
using ErrorExitFn = std::int32_t (*)();

struct ErrorGlobalsState {
    ErrorDisplayFn g_936268 = nullptr;
    ErrorExitFn g_93626c = nullptr;
    char g_a84640[0x1000]{};
};

[[nodiscard]] ErrorGlobalsState& Error_GetGlobals();
void Error_ResetGlobalsForTest();

} // namespace igi::retail::error
