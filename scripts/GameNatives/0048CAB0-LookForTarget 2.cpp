// IGI native provenance: LookForTarget - 0x0048CAB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048CAB0-LookForTarget.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LookForTarget.h"
int* LookForTarget(void* context, int* arg2)

{
    char* item_28 = "Combat - LookForTarget";
    int item_2c = 0xffffffff;
    void* item_30 = context;
    void* dest_1 = arg2[0xa] + 8;
    *(context + 0x3cdc) = 0x3f800000;
    FX_SmokeEmit_17(context, dest_1);

    if (!*(context + 0x3de4))
        return AI_CombatTarget_2(context, arg2);

    void item_18;
    AI_UpdatePatrol_3(&item_18, context);
    AI_UpdateOrders_3(context, &item_18, 0.0349065848f, 0);
    return FX_FireBallUpdate_18(context, arg2, arg2[7], 0);
}
