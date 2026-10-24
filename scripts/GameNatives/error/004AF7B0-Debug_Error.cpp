// IGI native provenance: Debug_Error - 0x004AF7B0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF7B0-Debug_Error.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Debug_Error.hpp"
int Debug_Error(char* context)

// Sig: void Debug_Error(char* format, ...)
// Note: Shows a fatal-error dialog: printf-style FORMAT STRING + variadic args
// (e.g. push "Level_Load(): Couldn't load script: %s"; push scriptPath; call).
// Param: format = printf-style string, followed by substitution args. Variadic
// - proven by retail call sites (add stack_ptr,8 cleanup) and vsprintf helper
// fcn.004A5702.
{
    if (g_936268)
    {
        sprintf(0xa84640, "Fatal error:\n\n");
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
        g_936268(0xa84640);
    }

    int result = g_93626c;

    if (!result)
        return result;

    /* tailcall */
    return result();
}
