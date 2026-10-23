// IGI native provenance: AIAction_MoveToEvent - 0x0044D7D0 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D7D0-AIAction_MoveToEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_MoveToEvent.h"
int AIAction_MoveToEvent(int context, float arg2 )

// Sig: void AIAction_MoveToEvent(int eventId, int flags)
// Note: Moves the AI toward the source position of an EVENT it received.
// Params: eventId = AIEVENT_* whose position to move to, flags = AIACTIONFLAG
// (NONE=0/PUSHABLE=1). Use in event handlers (gunshot heard -> move there).
{
    int* data_1 = AI_CheckVisibility_2(g_5bda58);
    Env_ScanViewCone_12(&data_1[2]);
    int item_2c = 0;
    float item_30 = arg2;
    data_1[9] = Script_GetArg(item_30, item_2c);
    AI_SelectTarget(sub_48dfe0, data_1, Script_GetArg(arg2, 1));
    int item_18 = 2;
    int item_10 = 0;
    int item_c = 0x40000000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
