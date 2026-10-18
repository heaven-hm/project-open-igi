// IGI native provenance: Display_SetGammaRamp_10 - 0x004964A0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004964A0-Display_SetGammaRamp_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_10.h"
int Display_SetGammaRamp_10(void* context, int* arg2, char arg3)

{
    int* flagP = arg2;
    void* data_1 = context;
    int* source_val = flagP[9];
    unsigned int context_3 = (flagP[6] * *(flagP + 0x16) * flagP[8]) >> 3;
    arg2 = source_val;

    if (data_1)
    {
        int* entity_1 = *(data_1 + 0x10);
        arg2 = entity_1;
        source_val = entity_1;
    }

    if (arg3)
    {
        int data_3 = (*(*source_val + 0x50))(source_val);
        data_3 = !data_3;
        return data_3;
    }

    int item_8;
    int item_4;
    int data_5 = (*(*source_val + 0x2c))(source_val, 0, context_3, &item_4, &arg3, &item_8, &context, 0);

    if (!data_5)
    {
        int data_6 = arg3;

        if (context_3 <= data_6)
        {
            int source_2;
            int dest_2;
            dest_2 = memcpy(item_4, flagP[0xb], data_6 & 0xfffffffc);
            memcpy(dest_2, source_2, data_6 & 3);
            data_6 = arg3;
            source_val = arg2;
            context_3 -= data_6;
        }

        if (context_3 > 0)
        {
            void* entity_6 = context;

            if (context_3 <= entity_6)
            {
                int source_5;
                int dest_4;
                dest_4 = memcpy(item_8, flagP[0xb] + data_6, entity_6 & 0xfffffffc);
                memcpy(dest_4, source_5, entity_6 & 3);
                data_6 = arg3;
                source_val = arg2;
            }
        }

        data_5 = (*(*source_val + 0x4c))(source_val, item_4, data_6, item_8, context);

        if (!data_5)
        {
            data_5 = 1;
            return data_5;
        }
    }

    data_5 = 0;
    return data_5;
}
