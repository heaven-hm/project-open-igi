// IGI native provenance: AIAction_FireAtNode - 0x0044D580 - CFG score 95.8% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D580-AIAction_FireAtNode.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIAction_FireAtNode.h"
int AIAction_FireAtNode(long double context , long double arg2 , int arg3, float arg4 )

// Sig: void AIAction_FireAtNode(int nodeId, int p1, int p2, int p3, int p4)
// Note: Makes the AI fire AT a nav-node position. Params: nodeId = target node
// id, remaining 4 args = firing stance/behaviour values observed in retail AI
// scripts (map before inventing values). Use in combat handlers when target is
// a position.
{
    int* data_1 = AI_CheckVisibility_2(g_5bda58);
    int item_30 = 0;
    data_1[2] = Script_GetArg(arg4, item_30);
    Profile_ValueSet(arg4, 1.40129846e-45.0f);
    data_1[3] = context * 30.0;
    Profile_ValueSet(arg4, 2.80259693e-45.0f);
    data_1[4] = arg2 * 30.0;
    data_1[6] = Script_GetArg(arg4, 3);
    AI_SelectTarget(sub_48eca0, data_1, Script_GetArg(arg4, 4));
    int item_18 = 5;
    int item_10 = 0;
    int item_c = 0x40140000;
    char* item_8 = &g_567c74;
    memcpy(arg3, &item_18, 0x18);
    return arg3;
}
