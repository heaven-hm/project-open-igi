// IGI native provenance: Soldier_DeathStateA - 0x0045C5A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045C5A0-Soldier_DeathStateA.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045C5A0-Soldier_DeathStateA.hpp"
void* Soldier_DeathStateA(void* context, void* arg2)

{
    void* data_val = Table_GetAt(arg2 + 0x4c4, 0);

    if (data_val)
    {
        Script_ParseEnumInt32_13(data_val);
        Script_ExecuteLevelScript_12(arg2 + 0x4c4, nullptr, nullptr);
    }

    Script_EvaluateCondition_12(arg2);
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 1;
    Bones_PlayAnimation(arg2 + 0x68, 0, 0x18, 0, 0.0f);
    Soldier_DeathFallSolver(arg2 + 0xd38, arg2, *(arg2 + 0xcf8), 0x18);
    Human_ViewCam(arg2, 3);
    AI_MoveToNode_22(*(arg2 + 0x4ec));
    AI_TakeCover_20(arg2);
    AI_TakeCover_21(arg2, "ai_death_", 4);
    void* result = FX_TrailUpdate_15(arg2);
    *(context + 0x14) = sub_45c700;
    return result;
}
