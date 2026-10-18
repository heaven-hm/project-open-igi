// IGI native provenance: AI_TakeCover_21 - 0x004638A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004638A0-AI_TakeCover_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_TakeCover_21.h"
struct status_msg* AI_TakeCover_21(void* context, int arg2, int arg3)

{
    int* data_val = *(context + 0x840);

    if (data_val)
    {
        void* data_1 = *data_val;

        if (data_1)
            SoundSys_SetFrequency_7(data_1);

        Qtask_UpdateList(*(context + 0x840));
        *(context + 0x840) = 0;
    }

    unsigned int item_30 = Sys_FreeMemory_14(Level_SetMission_5(), arg3);
    int item_34 = arg2;
    char item_20[0x20];
    Game_DataSymbolLoad(&item_20, "%s%d");
    return SoundSys_SetMasterVolume_7(context, &item_20);
}
