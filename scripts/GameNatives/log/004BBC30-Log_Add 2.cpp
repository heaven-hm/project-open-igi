// IGI native provenance: Log_Add - 0x004BBC30 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004BBC30-Log_Add.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Log_Add.h"
int Log_Add(char* context)

// Sig: void Log_Add(char* format, ...)
// Note: Appends a formatted line to the engine log (variadic printf-style,
// giant 0x400 format buffer at stack_ptr+0x404). Only logs when log-enabled globals
// 0xA43EC8/0xA43ED4 are set. Param: format = printf-style string + substitution
// args. Gate your spammy logs behind the enable flags.
{
    int result = g_a43ed4;

    if (!result)
        return result;

    void item_400;
    void arg_8;
    String_VFormat(&item_400, context, &arg_8);
    return g_a43ed4(&item_400, g_a43ec8);
}
