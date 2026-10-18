// IGI native provenance: FX_RunToNodeD - 0x0048EC50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048EC50-FX_RunToNodeD.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_RunToNodeD.h"
int FX_RunToNodeD(int context, int* arg2)

{
    void* dest_val = arg2[0xa];
    int item_10 = *(dest_val + 8);
    char* item_14 = "Run To Node - %d";
    int item_18 = 0xffffffff;
    int item_1c = context;
    AI_SelectTarget_3(context);
    int dest_1 = *(dest_val + 8);
    *(context + 0x3e30) = dest_1;
    char result = AI_MoveToNode_3(context, dest_1, nullptr, 1);

    if (!result)
        return result;

    return AI_CombatTarget_2(context, arg2);
}
