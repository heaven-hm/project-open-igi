#include "igi/retail/error/error_show.hpp"

#include <cstdio>

namespace igi::retail::error {
namespace {

// Port-owned storage for retail 0x00936268 / 0x0093626C / 0x00A84640. Debug_Error
// (0x004AF7B0) reads the two callback pointers and formats into this buffer.
ErrorGlobalsState error_globals{};

} // namespace

// Native: String_VFormat; retail VA 0x004A5702; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004A5702-0x004A5753, 81 bytes, digest dfd43dec4ebf396e9deadda3a0e5ef20a4952db80910e284b9b29f1aaa78d203; CLEAN/string/004A5702-String_VFormat.c.
// Behavior/CFG: initializes a stack CRT _iobuf stream over `buffer` with count `0x7FFFFFFF` and flags `0x42` (_IOWRT | _IOSTRG), invokes the formatting engine `0x004A7720` (`_output`) with `format` and `args`, decrements `_cnt` and either flushes via `0x004A760B` (`_flsbuf`) when negative or NUL-terminates `*ptr = 0`, and returns the written character count. radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact unbounded CRT vsprintf semantics and NUL termination.
std::int32_t String_VFormat(char* buffer, const char* format, std::va_list args)
{
    std::int32_t item_20 = 0x7FFFFFFF;
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    const std::int32_t result = std::vsprintf(buffer, format, args);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

    item_20 -= 1;
    if (item_20 >= 0 && result >= 0)
    {
        buffer[result] = '\0';
    }
    return result;
}

// Native: sprintf; retail VA 0x004A53B3 - the statically linked MSVC CRT sprintf, not a game
// wrapper. Binary Ninja: 0x004A53B3-0x004A5405, 82 bytes, 4 blocks, 484 callsites, cdecl variadic
// int(char* buffer, const char* format, ...), callees _output (0x004A7720) and _flsbuf
// (0x004A760B). radare2: same 82 bytes, 4 blocks/4 edges, cdecl, 3 arguments. angr CFGFast: same
// entry, no unresolved jumps. The port defines no body for it; the header's `using ::sprintf`
// binds the name to the candidate CRT symbol, which is what retail's callsites use too.
//
// The fatal dispatcher at 0x004AF7B0 is Debug_Error, defined in
// engine/src/retail/core/retail_error.cpp. No Error_Show alias exists here: retail has one
// function at that address and the candidate exposes one symbol for it.

ErrorGlobalsState& Error_GetGlobals()
{
    return error_globals;
}

void Error_ResetGlobalsForTest()
{
    error_globals = ErrorGlobalsState{};
}

} // namespace igi::retail::error
