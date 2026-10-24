// IGI native provenance: GuardGenerator_Setup - 0x00487EE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00487EE0-GuardGenerator_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00487EE0-GuardGenerator_Setup.hpp"
int* GuardGenerator_Setup(long double context , void* arg2)

{
    if (Script_FormatString_6(arg2 + 0x20))
    {
        Script_SpawnNewTask_7(arg2 + 0x20, arg2);
        *(arg2 + 0x70) = context;
    }

    int* result;

    if (!*(arg2 + 0x70))
    {
        result = *(arg2 + 8);

        if (!*result || !result)
            *(arg2 + 0x72) = 0;
    }
    else
    {
        result = *(arg2 + 0x78);

        if (*(arg2 + 0x74) < result)
        {
            int* dest_1 = *(arg2 + 8);

            if (!*dest_1 || !dest_1)
            {
                FX_TrailUpdate_9(arg2);
                result = *(arg2 + 0x74) + 1;
                *(arg2 + 0x72) = 1;
                *(arg2 + 0x74) = result;
            }
            else
            {
                result = AI_FireWeapon_19(dest_1);

                if (result)
                {
                    result = AI_TakeCover_19(dest_1);

                    if (!result)
                    {
                        int* entity_2 = *(arg2 + 8);
                        int data_4 = *entity_2;
                        int data_5 = -(data_4);
                        QTask_FindType((data_5 - data_5) & entity_2);
                        FX_TrailUpdate_9(arg2);
                        result = *(arg2 + 0x74) + 1;
                        *(arg2 + 0x72) = 1;
                        *(arg2 + 0x74) = result;
                    }
                }
            }
        }
    }

    int* i = *(arg2 + 8);

    if (*i && i)
    {
        int temp_1 = g_afa7e0;

        do
        {
            int* source_1 = *i;
            void* data_13;

            if (!source_1)
                data_13 = nullptr;
            else
            {
                int data_10 = *source_1;
                int data_11 = -(data_10);
                data_13 = (data_11 - data_11) & source_1;
            }

            *((temp_1 << 2) + &g_afa6e0) = data_13;
            int temp_2 = temp_1 + 1;
            int data_14 = 0;
            g_afa7e0 = temp_2;
            data_14 = i[7];
            result = *((data_14 << 2) + &g_a970e0);

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

    return result;
}
