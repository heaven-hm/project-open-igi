// IGI native provenance: Radio_LoadParameters - 0x0043AF90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043AF90-Radio_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Radio_LoadParameters.h"
int* Radio_LoadParameters(long double context , void* arg2)

{
    int flagP = *(arg2 + 0x240);
    (*((FX_TrailUpdate() << 2) + &g_a970e0))(arg2);

    if (*(arg2 + 0x190))
        *(arg2 + 0x240) = 0;
    else if (Script_FormatString_6(arg2 + 0x244))
    {
        Script_SpawnNewTask_7(arg2 + 0x244, arg2);
        *(arg2 + 0x240) = context;
    }

    int* result = *(arg2 + 0x238);

    if (!*(arg2 + 0x240))
    {
        if (result)
        {
            int data_19 = *result;

            if (data_19)
                SoundSys_SetFrequency_7(data_19);

            result = Qtask_UpdateList(*(arg2 + 0x238));
            *(arg2 + 0x238) = 0;
        }

        if (flagP)
        {
            result = SoundSys_SetMasterVolume_7(arg2, arg2 + 0x2d4);

            if (result)
                return SoundSys_StopSoundSample_7(result, arg2 + 0x20);
        }
    }
    else if (!result)
    {
    label_43b02e:
        int data_9 = *(arg2 + 0x23c);

        if (data_9)
        {
            *(arg2 + 0x23c) = data_9 - 1;
            return data_9 - 1;
        }

        struct status_msg* data_14 =
            SoundSys_SetMasterVolume_7(arg2, (Sys_FreeMemory_14(Level_SetMission_5(), 4) << 4) + arg2 + 0x294);
        result = Sys_FreeMemory_14(Level_SetMission_5(), 0x96) + 0x96;
        *(arg2 + 0x23c) = result;

        if (data_14)
        {
            *(arg2 + 0x238) = QTask_LinkEventHandler_2(data_14);
            return SoundSys_StopSoundSample_7(data_14, arg2 + 0x20);
        }
    }
    else if (!*result)
    {
        if (result)
        {
            Qtask_UpdateList(result);
            *(arg2 + 0x238) = 0;
        }

        goto label_43b02e;
    }

    return result;
}
