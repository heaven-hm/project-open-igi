// IGI native provenance: Camera_UpdateTransform_23 - 0x004ABD42 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ABD42-Camera_UpdateTransform_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ABD42-Camera_UpdateTransform_23.hpp"
int Camera_UpdateTransform_23(int context, HANDLE arg2)

{
    if (context < g_c32540 && (&g_c32440)[context >> 5][(context & 0x1f) * 2] == 0xffffffff)
    {
        if (g_543e80 == 1)
        {
            if (!context)
                SetStdHandle(STD_INPUT_HANDLE, arg2);
            else if (context == 1)
                SetStdHandle(STD_OUTPUT_HANDLE, arg2);
            else if (context == 2)
                SetStdHandle(STD_ERROR_HANDLE, arg2);
        }

        (&g_c32440)[context >> 5][(context & 0x1f) * 2] = arg2;
        return 0;
    }

    g_936068 = 0;
    g_936064 = 9;
    return 0xffffffff;
}
