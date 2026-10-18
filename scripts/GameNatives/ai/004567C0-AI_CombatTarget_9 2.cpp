// IGI native provenance: AI_CombatTarget_9 - 0x004567C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004567C0-AI_CombatTarget_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_9.h"
struct status_msg* AI_CombatTarget_9(void* context, int* arg2)

{
    void* source_val = arg2;
    void* flagP = *(source_val + 0x800);
    int item_8 = 0xe;
    int item_4 = 0xc;
    arg2 = 0xffffffff;
    void* data_val = Table_GetAt(source_val + 0x4c4, 0);

    if (data_val)
    {
        Script_ParseEnumInt32_13(data_val);
        Script_ExecuteLevelScript_12(source_val + 0x4c4, nullptr, nullptr);
    }

    Script_EvaluateCondition_12(source_val);
    *(source_val + 0x33d) = 0;
    *(source_val + 0x696) = 1;
    int context_1 = *(source_val + 0x2e8);
    int data_1 = FX_FragmentSimulate_14(source_val + 0x254, context_1);

    if (!flagP)
    {
    label_45685a:
        unsigned int data_3 = Sys_FreeMemory_14(Level_SetMission_5(), 2);
        int entity_1 = *(source_val + 0xd08);

        if (entity_1 >= 0)
        {
            if (entity_1 <= 1)
            {
                long double fpu0;
                int data_5 = AI_AlertAllies_16(fpu0, source_val, source_val + 0xd10);
                char f0_1 = data_5;
                data_5 = -(data_5);
                arg2 = data_5 - data_5 + 0x10;
            }
            else if (entity_1 == 2)
            {
                if (context_1 == 0xf || context_1 == 0x10)
                    arg2 = *(data_1 + (data_3 << 2) + 8);
                else
                    arg2 = (&item_8)[data_3];
            }
        }
    }
    else
    {
        int* flagP1 = *(flagP + 0x3ad0);

        if (flagP1 == 0xffffffff)
            goto label_45685a;

        arg2 = flagP1;

        if (flagP1 == 0xffffffff)
            goto label_45685a;
    }

    Bones_PlayAnimation(source_val + 0x68, 0, arg2, 0, 0.0f);
    Soldier_DeathFallSolver(source_val + 0xd28, source_val, *(source_val + 0xcf8), arg2);
    Human_ViewCam(source_val, 3);
    AI_MoveToNode_22(*(source_val + 0x4ec));
    AI_TakeCover_20(source_val);
    struct status_msg* result = AI_TakeCover_21(source_val, "ai_death_", 4);
    *(context + 0x14) = sub_456920;
    return result;
}
