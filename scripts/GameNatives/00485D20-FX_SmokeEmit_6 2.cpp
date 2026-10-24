// IGI native provenance: FX_SmokeEmit_6 - 0x00485D20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485D20-FX_SmokeEmit_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_SmokeEmit_6.h"
int FX_SmokeEmit_6(int context, int arg2)

{
    void* data_val = FX_FragmentSimulate_6(context, arg2);

    if (data_val)
    {
        void* entity_1 = g_bc2360;

        if (!entity_1)
            *(data_val + 0x1cc) = 0;
        else
            *(data_val + 0x1cc) = *(entity_1 + 0x68);

        *(data_val + 0x1d6) = 1;
        *(data_val + 0x1d0) = 0;
    }

    int temp_val = arg2 * 0x40b;
    void* source_1 = context + (temp_val << 2);
    Qtask_UpdateList(*(context + (temp_val << 2) + 0x20));
    *(source_1 + 0x20) = 0;
    *(source_1 + 0x40) = 0;
    int* source_2 = source_1 + 0x30;
    int i_1 = 3;
    int result;
    int i;

    do
    {
        result = Sys_ReadFileBlock_18(*source_2);
        *source_2 = 0;
        source_2 = &source_2[1];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    return result;
}
