// IGI native provenance: Warning_Show - 0x004AF810 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF810-Warning_Show.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AF810-Warning_Show.hpp"
int Warning_Show(char* context)

// Sig: void Warning_Show(char* format, ...)
// Note: Shows a warning dialog - printf-style variadic like ErrorShow (retail:
// push str."QTasktype \"%s\"..."; push data_val; call -> add stack_ptr,8). Param: format =
// printf-style string + substitution args.
{
    int result = g_936274;

    if (!result)
        return result;

    sprintf(0xa84640, "Warning:\n\n");
    int dest_1 = 0xa84640;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_1;
        dest_1 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    void arg_8;
    String_VFormat(~i + 0xa8463f, context, &arg_8);
    return g_936274(0xa84640);
}
