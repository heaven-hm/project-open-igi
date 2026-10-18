// IGI native provenance: AI_AiDeath - 0x004546C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004546C0-AI_AiDeath.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AiDeath.hpp"
void* AI_AiDeath(void* context, void* arg2)

{
    void* flagP = *(arg2 + 0x800);
    void* data_val = Table_GetAt(arg2 + 0x4c4, 0);

    if (data_val)
    {
        Script_ParseEnumInt32_13(data_val);
        Script_ExecuteLevelScript_12(arg2 + 0x4c4, nullptr, nullptr);
    }

    Script_EvaluateCondition_12(arg2);
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 1;
    void* data_2 = FX_FragmentSimulate_14(arg2 + 0x254, *(arg2 + 0x2e8));
    int* dest_1;

    if (!flagP)
        dest_1 = *(data_2 + 8);
    else
    {
        int* flagP1 = *(flagP + 0x3ad0);

        if (flagP1 == 0xffffffff)
            dest_1 = *(data_2 + 8);
        else
        {
            dest_1 = flagP1;

            if (dest_1 == 0xffffffff)
                dest_1 = *(data_2 + 8);
        }
    }

    Bones_PlayAnimation(arg2 + 0x68, 0, dest_1, 0, 0.0f);
    Soldier_DeathFallSolver(arg2 + 0xd38, arg2, *(arg2 + 0xcf8), dest_1);
    Human_ViewCam(arg2, 3);
    AI_MoveToNode_22(*(arg2 + 0x4ec));
    AI_TakeCover_20(arg2);
    AI_TakeCover_21(arg2, "ai_death_", 4);
    void* result = FX_TrailUpdate_15(arg2);
    *(context + 0x14) = sub_4547b0;
    return result;
}
