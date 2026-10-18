// IGI native provenance: DynObj_LinkDynamicCube - 0x004C02C0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004C02C0-DynObj_LinkDynamicCube.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C02C0-DynObj_LinkDynamicCube.hpp"
int DynObj_LinkDynamicCube(int* context, int arg2, int arg3)

{
    int i_2;
    int i_1 = i_2;
    int data_val = g_a44314;
    int context_val = 0;

    if (!data_val)
        return DynObj_UpdateDynTree(context, arg2, arg3);

    i_1 = 0;

    if (context[1] <= 0)
        return data_val;

    int i;

    do
    {
        int* j_1 = *(*context + context_val);

        for (int* j = *j_1; j; j = *j)
        {
            if (g_a44314(j_1[2], j_1[3], g_a44318))
                arg2(j_1[2], j_1[3], arg3);

            j_1 = j;
        }

        i = i_1 + 1;
        context_val += 0xc;
        i_1 = i;
    } while (i < context[1]);

    return i;
}
