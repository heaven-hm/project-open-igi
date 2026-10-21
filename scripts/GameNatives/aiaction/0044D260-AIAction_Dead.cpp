// IGI native provenance: AIAction_Dead - 0x0044D260 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D260-AIAction_Dead.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_Dead.h"
int AIAction_Dead(int context, float arg2 )

// Sig: void AIAction_Dead(int args)
// Note: Runs the DEATH handling action for the AI (plays death, releases
// tasks). Param: args = death context value passed by retail scripts. Use in
// AIEVENT_DEAD handler.
{
    int data_1 = Script_GetArg(arg2, 0);
    int item_30 = 0;
    AI_SelectTarget(sub_48ab70, item_30, data_1);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
