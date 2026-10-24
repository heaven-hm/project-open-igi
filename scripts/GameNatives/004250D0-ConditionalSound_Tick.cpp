// IGI native provenance: ConditionalSound_Tick - 0x004250D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004250D0-ConditionalSound_Tick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004250D0-ConditionalSound_Tick.hpp"
int* ConditionalSound_Tick(long double context , void* arg2)

{
    if (Script_FormatString_6(arg2 + 0x20))
    {
        *(arg2 + 0xb1) = *(arg2 + 0xb0);
        Script_SpawnNewTask_7(arg2 + 0x20, arg2);
        *(arg2 + 0xb0) = context;
    }

    int* result = Menu_DrawWidget_15(arg2);
    result = *(arg2 + 0xa9);

    if (!result)
    {
    label_42512e:
        result = *(arg2 + 0xb0);

        if (!result)
            result = ConditionalSound_Stop(arg2);
        else
        {
            result = *(arg2 + 0xb1);

            if (!result)
                result = ConditionalSound_Start(arg2);
        }

        int* i = *(arg2 + 8);

        if (*i && i)
        {
            int temp_1 = g_afa7e0;

            do
            {
                int* source_1 = *i;
                void* data_5;

                if (!source_1)
                    data_5 = nullptr;
                else
                {
                    int data_2 = *source_1;
                    int data_3 = -(data_2);
                    data_5 = (data_3 - data_3) & source_1;
                }

                *((temp_1 << 2) + &g_afa6e0) = data_5;
                int temp_2 = temp_1 + 1;
                int data_6 = 0;
                g_afa7e0 = temp_2;
                data_6 = i[7];
                result = *((data_6 << 2) + &g_a970e0);

                if (result)
                {
                    result = result(i);
                    temp_2 = g_afa7e0;
                }

                i = *((temp_2 << 2) + &g_afa6dc);
                temp_1 = temp_2 - 1;
                g_afa7e0 = temp_1;
            } while (i);
        }
    }
    else
    {
        result = *(arg2 + 0x124);

        if (!result)
            goto label_42512e;
    }

    return result;
}
