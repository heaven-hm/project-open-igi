// IGI native provenance: Switch_Setup - 0x004450F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004450F0-Switch_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Switch_Setup.hpp"
void* Switch_Setup(long double context , void* arg2)

{
    int* i = *(arg2 + 8);

    if (*i && i)
    {
        int temp_1 = g_afa7e0;

        do
        {
            int* dest_1 = *i;
            void* data_4;

            if (!dest_1)
                data_4 = nullptr;
            else
            {
                int data_1 = *dest_1;
                int data_2 = -(data_1);
                data_4 = (data_2 - data_2) & dest_1;
            }

            *((temp_1 << 2) + &g_afa6e0) = data_4;
            int temp_2 = temp_1 + 1;
            int data_5 = 0;
            g_afa7e0 = temp_2;
            data_5 = i[7];
            int data_6 = *((data_5 << 2) + &g_a970e0);

            if (data_6)
            {
                data_6(i);
                temp_2 = g_afa7e0;
            }

            i = *((temp_2 << 2) + &g_afa6dc);
            temp_1 = temp_2 - 1;
            g_afa7e0 = temp_1;
        } while (i);
    }

    if (Script_FormatString_6(arg2 + 0x1d0))
    {
        Script_SpawnNewTask_7(arg2 + 0x1d0, arg2);
        *(arg2 + 0x1c0) = context;
    }

    void* result = g_bc2360;

    if (*(result + 0x68) > *(arg2 + 0x1c8) + 2)
    {
        char entity_1 = *(arg2 + 0x1c4);
        *(arg2 + 0x1c4) = 0;
        *(arg2 + 0x1c5) = entity_1;
    }

    result = *(arg2 + 0x1c0);

    if (result != *(arg2 + 0x1c1) || *(arg2 + 0x1c4) != *(arg2 + 0x1c5)
        || *(arg2 + 0x1c2) != *(arg2 + 0x1c3))
    {
        int data_8;

        if (*(arg2 + 0x1c2))
            data_8 = 4;
        else if (!result)
        {
            int data_11 = 0;
            data_11 = *(arg2 + 0x1c4);
            data_8 = data_11 + 2;
        }
        else if (!*(arg2 + 0x1c4))
        {
            struct status_msg* data_10 = SoundSys_SetMasterVolume_7(arg2, "switch");

            if (data_10)
                SoundSys_StopSoundSample_7(data_10, arg2 + 0x20);

            data_8 = 0;
        }
        else
            data_8 = 1;

        result = DynObj_CreateMagicObject_5(arg2, DynObj_Missing(((data_8 + 0x17) << 4) + arg2));
        result = *(arg2 + 0x1c0);
        char entity_3 = *(arg2 + 0x1c2);
        *(arg2 + 0x1c1) = result;
        *(arg2 + 0x1c3) = entity_3;
    }

    return result;
}
