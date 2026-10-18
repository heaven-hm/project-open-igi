// IGI native provenance: AIAction_LookAtEvent - 0x0044D870 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D870-AIAction_LookAtEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044D870-AIAction_LookAtEvent.hpp"
int AIAction_LookAtEvent(long double context , int arg2, float arg3 )

// Sig: void AIAction_LookAtEvent(int eventId, int flags)
// Note: Turns the AI to LOOK toward an event's source without moving. Params:
// eventId = AIEVENT_* value, flags = AIACTIONFLAG (NONE=0/PUSHABLE=1).
{
    int* data_1 = AI_CheckVisibility_2(g_5bda58);
    Env_ScanViewCone_12(&data_1[2]);
    float item_2c = 0.0f;
    float item_30 = arg3;
    Profile_ValueSet(item_30, item_2c);
    data_1[8] = context;
    AI_SelectTarget(sub_48e080, data_1, Script_GetArg(arg3, 1));
    int item_18 = 2;
    int item_10 = 0;
    int item_c = 0x40000000;
    char* item_8 = &g_567c74;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
