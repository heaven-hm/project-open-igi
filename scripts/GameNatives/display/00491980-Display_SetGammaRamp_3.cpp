// IGI native provenance: Display_SetGammaRamp_3 - 0x00491980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491980-Display_SetGammaRamp_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_3.hpp"
int Display_SetGammaRamp_3(void* context, char arg2, int arg3)

{
    int entity_val;
    int item_4 = entity_val;
    int result = 0;
    int i = 0;
    int result_1 = 0;

    if (g_c10384 <= 0)
        return 0;

    int* source_val = &g_bcae28;

    do
    {
        int data_1 = 1;

        if (!context)
        {
        label_4919f2:

            if (arg3)
            {
                arg3(&source_val[-2]);
                result = result_1;
            }

            result += 1;
            result_1 = result;
        }
        else
        {
            if (1 & arg2 && *(context + 4) != source_val[-1])
                data_1 = 0;

            if (arg2 & 2 && *(context + 8) != *source_val)
                data_1 = 0;

            if (arg2 & 4 && *(context + 0x10) != source_val[2])
                data_1 = 0;

            if ((!(arg2 & 8) || *(context + 0x18) == source_val[4]) && data_1)
                goto label_4919f2;
        }

        i += 1;
        source_val = &source_val[0x47];
    } while (i < g_c10384);

    return result;
}
