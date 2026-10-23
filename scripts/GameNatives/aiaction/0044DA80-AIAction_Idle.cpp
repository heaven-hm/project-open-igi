// IGI native provenance: AIAction_Idle - 0x0044DA80 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044DA80-AIAction_Idle.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_Idle.h"
int AIAction_Idle(int context, float arg2 )

// Sig: void AIAction_Idle(int args)
// Note: Puts the AI into IDLE (stands, resumes idle animation countdown).
// Param: idle context value (retail scripts pass their idle value). Use in
// AIEVENT_IDLE handler instead of Patrol for stationary guards.
{
    int data_1 = Script_GetArg(arg2, 0);
    int item_30 = 0;
    AI_SelectTarget(sub_48e510, item_30, data_1);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
