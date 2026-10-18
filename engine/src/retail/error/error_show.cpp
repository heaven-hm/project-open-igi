#include "igi/retail/error/error_show.hpp"

#include <cstdio>
#include <cstring>

namespace igi::retail::error {
namespace {

ErrorGlobalsState error_globals{};


} // namespace

// Native: String_VFormat; retail VA 0x004A5702; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x004A5702-0x004A5753, 81 bytes, digest dfd43dec4ebf396e9deadda3a0e5ef20a4952db80910e284b9b29f1aaa78d203; CLEAN/string/004A5702-String_VFormat.c.
// Behavior/CFG: initializes a stack CRT _iobuf stream over `buffer` with count `0x7FFFFFFF` and flags `0x42` (_IOWRT | _IOSTRG), invokes the formatting engine `0x004A7720` (`Camera_ApplyWideScreen_11`) with `format` and `args`, decrements `_cnt` and either flushes via `0x004A760B` (`Camera_ApplyHandiCam_11`) when negative or NUL-terminates `*ptr = 0`, and returns the written character count. radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100% structural agreement.
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


// Native: Game_DataSymbolLoad; retail VA 0x004A53B3; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004A53B3-0x004A5405, 82 bytes, digest 903e5756370f8719b677989dccf6159c0e5d23a86e6b57b11d9e7669453b7042; CLEAN/game/004A53B3-Game_DataSymbolLoad.c.
// Behavior/CFG: initializes a stack CRT _iobuf stream over `buffer` with count `0x7FFFFFFF` and flags `0x42` (_IOWRT | _IOSTRG), passes `&arg_10h` (variadic argument list) to `0x004A7720` (`Camera_ApplyWideScreen_11`), NUL-terminates `*ptr = 0`, and returns the formatted length. radare2 4 blocks/4 edges; Binary Ninja 4 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact variadic sprintf behavior used across retail path builders and Error_Show.
std::int32_t Game_DataSymbolLoad(char* buffer, const char* format, ...)
{
    std::va_list args;
    va_start(args, format);
    const std::int32_t result = String_VFormat(buffer, format, args);
    va_end(args);
    return result;
}

// Native: Error_Show; retail VA 0x004AF7B0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004AF7B0-0x004AF809, 89 bytes, digest 1c20410541c863f90a17d98f1088afa496d65e0a6a62a8f4447fbd46ac37d72e; CLEAN/error/004AF7B0-Error_Show.c.
// Behavior/CFG: if callback `g_936268` is non-null, formats `"Fatal error:\n\n"` into global buffer `0x00A84640` via Game_DataSymbolLoad (0x004A53B3), scans `0x00A84640` with `repne scasb` (`i = 0xFFFFFFFF`) to find its NUL terminator (`~i - 1`), formats `format` and variadic arguments at `0x00A84640 + (~i - 1)` via String_VFormat (0x004A5702), and calls `g_936268(0x00A84640)`. Then reads callback `g_93626c`; if zero returns 0, otherwise tail-calls `g_93626c()`. radare2 5 blocks/5 edges; Binary Ninja 5 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact branch, `repne scasb` length arithmetic, buffer append, and callback tailcall.
std::int32_t Error_Show(const char* format, ...)
{
    if (error_globals.g_936268 != nullptr)
    {
        Game_DataSymbolLoad(error_globals.g_a84640, "Fatal error:\n\n");
        const char* dest_1 = error_globals.g_a84640;
        std::uint32_t i = 0xFFFFFFFFU;

        while (i != 0U)
        {
            const bool flag_0_1 = (*dest_1 != '\0');
            dest_1 += 1;
            i -= 1U;
            if (!flag_0_1)
            {
                break;
            }
        }

        const std::size_t prefix_len = static_cast<std::size_t>(~i - 1U);
        std::va_list args;
        va_start(args, format);
        String_VFormat(error_globals.g_a84640 + prefix_len, format, args);
        va_end(args);
        error_globals.g_936268(error_globals.g_a84640);
    }

    const ErrorExitFn result = error_globals.g_93626c;
    if (result == nullptr)
    {
        return 0;
    }

    return result();
}

ErrorGlobalsState& Error_GetGlobals()
{
    return error_globals;
}

void Error_ResetGlobalsForTest()
{
    error_globals = ErrorGlobalsState{};
}

} // namespace igi::retail::error
