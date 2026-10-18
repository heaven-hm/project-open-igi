// IGI native provenance: AI_UpdatePatrol_2 - 0x00450F10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00450F10-AI_UpdatePatrol_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00450F10-AI_UpdatePatrol_2.hpp"
int AI_UpdatePatrol_2(void* context, void* arg2)

{
    void* flagP = context;
    char context_val = 0;
    AI_CheckSector(flagP);
    int* i_1 = *(arg2 + 8);
    int result = 0;

    for (int* i = i_1[1]; i; i = i[1])
    {
        if (!context_val)
        {
            context_val = 1;
            context = i_1[4];
        }

        i_1[4] = context;
        int temp_2 = *(i_1[1] + 4);
        i_1[1] = 0;
        *i_1 = 0;
        int* item_24_1;
        int* item_20_1;

        if (temp_2)
        {
            item_20_1 = flagP + 0x8fc;
            Camera_ApplyLineFilter_33(item_20_1, i_1);
            item_24_1 = Level_SetMission_5();
            i_1[3] = (Sys_AllocateMemory_14(item_24_1) + 1.0f) * 0.200000003f * 30.0f;
        }
        else
        {
            item_20_1 = flagP + 0x8fc;
            Camera_ApplyLineFilter_33(item_20_1, i_1);
            item_24_1 = Level_SetMission_5();
            i_1[3] = (Sys_AllocateMemory_14(item_24_1) + 1.0f) * 0.200000003f * 30.0f;
        }

        result = i_1[5](flagP, i_1, item_24_1, item_20_1, i_1);
        i_1 = i;
    }

    return result;
}
