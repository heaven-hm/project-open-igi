// IGI native provenance: AIAction_PlayAnimation - 0x0044D6E0 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D6E0-AIAction_PlayAnimation.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_PlayAnimation.h"
int AIAction_PlayAnimation(int context, float arg2 )

// Sig: void AIAction_PlayAnimation(int animationId, int flags)
// Note: Plays an animation on the AI. Params: animationId = animation entry id
// (see AIFunction_AddAnimationEntry), flags = AIACTIONFLAG (NONE=0/PUSHABLE=1).
{
    int* data_1 = AI_CheckVisibility_2(g_5bda58);
    int data_2 = Script_GetArg(arg2, 0);
    int item_30 = 1;
    data_1[2] = data_2;
    AI_SelectTarget(sub_48dea0, data_1, Script_GetArg(arg2, item_30));
    int item_18 = 2;
    int item_10 = 0;
    int item_c = 0x40000000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
