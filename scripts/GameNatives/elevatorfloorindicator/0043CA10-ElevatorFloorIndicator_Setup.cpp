// IGI native provenance: ElevatorFloorIndicator_Setup - 0x0043CA10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043CA10-ElevatorFloorIndicator_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043CA10-ElevatorFloorIndicator_Setup.hpp"
struct status_msg* ElevatorFloorIndicator_Setup(long double context , int arg2)

{
    int* i = *(arg2 + 8);

    if (*i && i)
    {
        int temp_1 = g_afa7e0;

        do
        {
            int* source_1 = *i;
            void* data_4;

            if (!source_1)
                data_4 = nullptr;
            else
            {
                int data_1 = *source_1;
                int data_2 = -(data_1);
                data_4 = (data_2 - data_2) & source_1;
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

    int context_val;

    if (!Script_FormatString_6(arg2 + 0x1a4))
        context_val = arg2;
    else
    {
        Script_SpawnNewTask_7(arg2 + 0x1a4, arg2);
        context_val = context;
    }

    int source_4;

    if (!Script_FormatString_6(arg2 + 0x1f4))
        source_4 = arg2;
    else
    {
        Script_SpawnNewTask_7(arg2 + 0x1f4, arg2);
🚫🚫🚫        int item_8_2 = /* int item_8_2 = (unimplemented  {fistp qword [stack_ptr+0x14], fp0}).d */;
        /* unimplemented  {fistp qword [stack_ptr+0x14], fp0} */
        source_4 = item_8_2;
    }

    char* data_9 = arg2 + 0x170;

    if (!context_val)
        data_9 = arg2 + 0x180;

    struct status_msg* result = DynObj_CreateMagicObject_5(arg2, DynObj_Missing(data_9));

    if (source_4)
    {
        result = SoundSys_SetMasterVolume_7(Script_FormatString(), arg2 + 0x190);

        if (result)
            return SoundSys_StopSoundSample_7(result, arg2 + 0x20);
    }

    return result;
}
