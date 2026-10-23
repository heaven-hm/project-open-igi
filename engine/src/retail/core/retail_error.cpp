#include "igi/retail/core/retail_error.hpp"

#include "igi/retail/error/error_show.hpp"

#include <cstdarg>
#include <cstddef>

// Native: Debug_Error; retail VA 0x004AF7B0; x86 cdecl int32_t(const char* format, ...).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja (89 bytes, 5 blocks, 5 edges, 29 instructions), Radare2 6.2.0 (89 bytes, 5 blocks, 5 edges, cdecl), angr CFGFast (exact entry; 10 blocks / 11 edges under the retail initialized-callback model, no unresolved jumps).
// Behavior/CFG: report-then-dispatch; when g_936268 is set it writes "Fatal error:\n\n" into 0x00A84640, appends the printf-formatted message at the first NUL via String_VFormat, and calls g_936268(0x00A84640); it then reads g_93626C, returns EAX=0 when that callback is null (`retn` at 0x004AF808), and otherwise tail-dispatches through it (`jmp eax` at 0x004AF806).
// Status/limits: STRUCTURALLY_VERIFIED; compile-checked with MinGW x86, no runtime evidence. The candidate does not install g_936268/g_93626C, so its current runtime path is silent; the installed retail callbacks (0x004988D0 MessageBoxA, 0x00498930 DebugBreak+exit) remain unported.
//
// Detail: the raw retail entry has a genuine return path. Its callers examined at 0x005144F7 ->
// 0x00514504, 0x005146F1 -> 0x005146F9, 0x004D2425 -> 0x004D242D, 0x004D2434 -> 0x004D243C,
// 0x00415B65 -> 0x00415B6D and 0x0044DFE8 -> 0x0044DFF0 append their own `jmp $`, and the
// candidate callers in terrain_quadtree.cpp append halt_forever(), so the fatal call sequence
// stays terminal without moving the self-loop into this function. The callbacks are zero in the
// file image; 0x00498900 installs g_936268=0x004988D0 and g_93626C=0x00498930, and the exit
// callback's CRT exit(1) -> ExitProcess(1) path never returns once installed. Error_GetGlobals
// exposes the port-owned storage for retail 0x00936268/0x0093626C/0x00A84640, shared with
// Error_Show in engine/src/retail/error/error_show.cpp (the same retail VA).
extern "C" int Debug_Error(const char* format, ...)
{
    namespace error = igi::retail::error;

    // Retail reads the single pair of callbacks at 0x00936268/0x0093626C and formats into the
    // single buffer at 0x00A84640. Error_GetGlobals exposes the port-owned storage for exactly
    // those retail globals, shared with Error_Show (the raw body of the same retail VA).
    error::ErrorGlobalsState& globals = error::Error_GetGlobals();

    if (globals.g_936268 != nullptr)
    {
        error::sprintf(globals.g_a84640, "Fatal error:\n\n");

        // Equivalent of the retail `repne scasb` NUL scan: prefix_len = strlen(g_a84640).
        std::size_t prefix_length = 0;
        while (globals.g_a84640[prefix_length] != '\0')
        {
            ++prefix_length;
        }

        std::va_list arguments;
        va_start(arguments, format);
        error::String_VFormat(globals.g_a84640 + prefix_length, format, arguments);
        va_end(arguments);

        globals.g_936268(globals.g_a84640);
    }

    // Retail 0x004AF7FD-0x004AF808: `mov eax,[0x0093626C]; test eax,eax; je retn`.
    // With no exit callback installed the raw entry returns 0.
    const error::ErrorExitFn exit_callback = globals.g_93626c;
    if (exit_callback == nullptr)
    {
        return 0;
    }

    // Retail 0x004AF806 is `jmp eax`, so the callback returns directly to Debug_Error's caller
    // and its result becomes this function's result.
    return exit_callback();
}
