// IGI native provenance: AlarmLight_LoadParameters - 0x0043A270 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043A270-AlarmLight_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AlarmLight_LoadParameters.h"
struct status_msg* AlarmLight_LoadParameters(long double context , void* arg2)

{
    char context_val = *(arg2 + 0x1a0);
    struct status_msg* result;

    if (!context_val || *(arg2 + 0x1a1))
    {
        result = *(arg2 + 0x1f4);

        if (result)
        {
            result = QTask_FindType(result);
            *(arg2 + 0x1f4) = 0;
        }
    }
    else
    {
        result = *(arg2 + 0x1f4);

        if (!result)
        {
            result = SoundSys_SetMasterVolume_7(arg2, arg2 + 0x160);
            *(arg2 + 0x1f4) = result;
        }
    }

    result = *(arg2 + 0x1a1);

    if (!result)
    {
        result = Script_FormatString_6(arg2 + 0x1a4);

        if (result)
        {
            result = Script_SpawnNewTask_7(arg2 + 0x1a4, arg2);
            *(arg2 + 0x1a0) = context;
        }

        result = *(arg2 + 0x1a0);

        if (!context_val)
        {
            if (result)
                result = DynObj_CreateMagicObject_5(arg2, DynObj_Missing(arg2 + 0x140));
        }
        else if (!result)
            result = DynObj_CreateMagicObject_5(arg2, DynObj_Missing(arg2 + 0x130));

        int* i = *(arg2 + 8);

        if (*i && i)
        {
            int temp_2 = g_afa7e0;

            do
            {
                int* dest_2 = *i;
                void* data_8;

                if (!dest_2)
                    data_8 = nullptr;
                else
                {
                    int data_5 = *dest_2;
                    int data_6 = -(data_5);
                    data_8 = (data_6 - data_6) & dest_2;
                }

                *((temp_2 << 2) + &g_afa6e0) = data_8;
                int temp_3 = temp_2 + 1;
                int data_9 = 0;
                g_afa7e0 = temp_3;
                data_9 = i[7];
                result = *((data_9 << 2) + &g_a970e0);

                if (result)
                {
                    result = result(i);
                    temp_3 = g_afa7e0;
                }

                i = *((temp_3 << 2) + &g_afa6dc);
                temp_2 = temp_3 - 1;
                g_afa7e0 = temp_2;
            } while (i);
        }

        void* source_1 = *(arg2 + 0xe8);

        if (source_1)
        {
            int entity_2 = 0;
            entity_2 = *(source_1 + 0x1c);
            return (*((entity_2 << 2) + &g_a970e0))(source_1);
        }
    }

    return result;
}
