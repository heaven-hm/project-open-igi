// IGI native provenance: QTask_CreateTask_3 - 0x00401E70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401E70-QTask_CreateTask_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401E70-QTask_CreateTask_3.hpp"
int QTask_CreateTask_3(void* context, int arg2, int arg3, int arg4)

{
    int data_4 = g_afa7e0;
    *((data_4 << 2) + &g_afa6e0) = context;
    int data_val = data_4 + 1;
    g_afa7e0 = data_val;
    short entity_val = *(context + 0x1c);

    if (entity_val == arg3)
    {
        int entity_2 = *(((arg2 * 0x180 + entity_val) << 2) + &g_a96ae0);

        if (entity_2)
        {
            entity_2(context, arg4);
            data_val = g_afa7e0;
        }
    }

    if (context == *((data_val << 2) + &g_afa6dc))
    {
        int* i = *(context + 8);

        if (*i && i)
        {
            do
            {
                int* source_1 = *i;
                void* temp_9;

                if (!source_1)
                    temp_9 = nullptr;
                else
                {
                    int temp_6 = *source_1;
                    int temp_7 = -(temp_6);
                    temp_9 = (temp_7 - temp_7) & source_1;
                }

                *((data_val << 2) + &g_afa6e0) = temp_9;
                g_afa7e0 = data_val + 1;
                QTask_CreateTask_3(i, arg2, arg3, arg4);
                data_val = g_afa7e0 - 1;
                g_afa7e0 = data_val;
                i = *((data_val << 2) + &g_afa6e0);
            } while (i);
        }
    }

    g_afa7e0 = data_val - 1;
    return data_val - 1;
}
