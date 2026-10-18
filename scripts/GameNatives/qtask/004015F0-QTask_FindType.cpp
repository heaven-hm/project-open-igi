// IGI native provenance: QTask_FindType - 0x004015F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004015F0-QTask_FindType.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004015F0-QTask_FindType.hpp"
int QTask_FindType(int* context)

{
    g_567c58 -= 1;
    int temp_1 = g_afa7e0;
    int data_val = 0;

    if (temp_1 > 0)
    {
        int* entity_1 = &g_afa6e0;

        do
        {
            if (*entity_1 == context)
            {
                int* temp_2 = *context;
                void* entity_5;

                if (!temp_2)
                    entity_5 = nullptr;
                else
                {
                    int entity_2 = *temp_2;
                    int entity_3 = -(entity_2);
                    entity_5 = (entity_3 - entity_3) & temp_2;
                }

                *((data_val << 2) + &g_afa6e0) = entity_5;
                break;
            }

            data_val += 1;
            entity_1 = &entity_1[1];
        } while (data_val < temp_1);
    }

    int data_1 = 0;
    data_1 = context[7];
    int data_2 = *((data_1 << 2) + &g_a97ce0);

    if (data_2)
        data_2(context);

    int* i_1 = context[4];

    for (int* i = i_1[1]; i; i = i[1])
    {
        QTask_FindType(i_1);
        i_1 = i;
    }

    int entity_6 = 0;
    entity_6 = context[7];
    int data_3 = *((entity_6 << 2) + &g_a976e0);

    if (data_3)
        data_3(context);

    if (context[6])
    {
        int data_5 = g_567c40;
        int entity_7 = 0;

        if (data_5 > 0)
        {
            do
            {
                int context_1 = g_567c38;
                int* data_8 = *(g_567c3c + (entity_7 << 2)) * g_567c48;

                if (*(data_8 + context_1) == context)
                    *(data_8 + context_1) = 0;

                entity_7 += 1;
            } while (entity_7 < data_5);
        }
    }

    QTask_AllocateEvent(context);

    if (!*(context + 0x1e))
        return Sys_ExecuteBytecode_2(context);

    int data_12 = 0;
    data_12 = context[7];
    int data_14 = data_12 * 0x30;
    int dest_1 = *(*(data_14 + 0xaf5f00) + context);
    *(data_14 + 0xaf5ef4) -= 1;
    int data_15 = 0;
    data_15 = context[7];
    int entity_10 = data_15 * 0x30;
    int data_16 = *(entity_10 + 0xaf5ef4);

    if (dest_1 == data_16)
        return data_16;

    int temp_4 = *(entity_10 + 0xaf5ef0);
    int flagP3 = *(temp_4 + (dest_1 << 2));
    int data_19 = *(temp_4 + (data_16 << 2)) * *(entity_10 + 0xaf5efc) + *(entity_10 + 0xaf5eec);
    int context_3 = *(*(entity_10 + 0xaf5f00) + data_19);
    *(temp_4 + (dest_1 << 2)) = *(temp_4 + (context_3 << 2));
    int entity_13 = 0;
    entity_13 = context[7];
    *(*(entity_13 * 0x30 + 0xaf5ef0) + (context_3 << 2)) = flagP3;
    int entity_15 = 0;
    entity_15 = context[7];
    *(*(entity_15 * 0x30 + 0xaf5f00) + data_19) = dest_1;
    return data_19;
}
