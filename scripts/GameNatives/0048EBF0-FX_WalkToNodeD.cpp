// IGI native provenance: FX_WalkToNodeD - 0x0048EBF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048EBF0-FX_WalkToNodeD.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048EBF0-FX_WalkToNodeD.hpp"
int FX_WalkToNodeD(int context, int* arg2)

{
    void* dest_val = arg2[0xa];
    int item_10 = *(dest_val + 8);
    char* item_14 = "Walk To Node - %d";
    int item_18 = 0xffffffff;
    int item_1c = context;
    AI_AlertAllies_3(context);
    int dest_1 = *(dest_val + 8);
    *(context + 0x3e30) = dest_1;
    char result = AI_MoveToNode_3(context, dest_1, nullptr, 1);

    if (!result)
        return result;

    return AI_CombatTarget_2(context, arg2);
}
