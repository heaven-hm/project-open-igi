// IGI native provenance: AI_MoveToNode_21 - 0x004637C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004637C0-AI_MoveToNode_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_21.h"
unsigned int AI_MoveToNode_21(void* context, int arg2, int arg3)

{
    void* dest_val = nullptr;
    unsigned int result = Frames_GetUpdateCounter();

    if (result >= *(context + 0x844))
    {
        int* data_val = *(context + 0x840);

        if (data_val)
        {
            dest_val = *data_val;

            if (dest_val)
                SoundSys_SetFrequency_7(dest_val);

            Qtask_UpdateList(*(context + 0x840));
            *(context + 0x840) = 0;
        }

        *(context + 0x844) =
            0x1e - Sys_AllocateMemory_14(Level_SetMission_5()) * -30.0f + Frames_GetUpdateCounter();
        int item_40_1 = Sys_FreeMemory_14(Level_SetMission_5(), arg3) + 1;
        int item_44_1 = arg2;
        char item_20[0x20];
        sprintf(&item_20, "%s%d");
        result = SoundSys_SetMasterVolume_7(*(context + 0x4ec), &item_20);

        if (dest_val)
        {
            result = QTask_LinkEventHandler_2(dest_val);
            *(context + 0x840) = result;
        }
    }

    return result;
}
