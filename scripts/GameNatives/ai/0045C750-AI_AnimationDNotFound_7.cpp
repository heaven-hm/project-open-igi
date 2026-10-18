// IGI native provenance: AI_AnimationDNotFound_7 - 0x0045C750 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045C750-AI_AnimationDNotFound_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AnimationDNotFound_7.hpp"
void* AI_AnimationDNotFound_7(void* context, void* arg2)

{
    int data_val = *(arg2 + 0xd0c);
    *(arg2 + 0x696) = 0;

    if (data_val == 0xffffffff)
        Human_StateAnimation(arg2, 2, 0.0f);
    else if (!Render_BindTexture_11(arg2 + 0x68, 0, data_val))
    {
        int item_8_2 = *(arg2 + 0xd0c);
        Warning_Show("Animation #%d not found");
        Human_StateAnimation(arg2, 2, 0.0f);
    }
    else
        FX_DebrisSpawn_14(arg2, *(arg2 + 0xd0c), 0, 0.0f);

    *(arg2 + 0xd0c) = 0;
    FX_TrailUpdate_15(arg2);
    *(context + 0x14) = AI_CombatTarget_16;
    return context;
}
