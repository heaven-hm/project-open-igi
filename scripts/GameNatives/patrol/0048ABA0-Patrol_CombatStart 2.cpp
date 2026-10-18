// IGI native provenance: Patrol_CombatStart - 0x0048ABA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048ABA0-Patrol_CombatStart.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Patrol_CombatStart.h"
int* Patrol_CombatStart(void* context, void* arg2)

{
    int* context_1 = *(arg2 + 0x28) + 8;

    if (!AI_MoveToNode_19(*(context + 0x14)))
    {
        if (!Locomotion_GetState(context, 2))
            *(arg2 + 0x18) = Combat_MobileC;
        else
            *(arg2 + 0x18) = 0x48c510;
    }
    else
        *(arg2 + 0x18) = Combat_MobileA;

    *(arg2 + 0x20) = FX_SmokeEmit_19;
    int entity_val = *(context + 0x3d44);
    *(context + 0x3dd0) = 0xffffffff;
    *(context + 0x3dd4) = entity_val;
    *(context + 0x3dd8) = 0xffffffff;
    *(context + 0x3e64) = 0;
    *(context + 0x3de8) = 0xffffffff;
    int* result_1 = **context_1;
    int* result = *result_1;

    if (result)
    {
        while (true)
        {
            int* entity_1 = result_1[0xb];

            if (entity_1)
            {
                void* entity_2 = *entity_1;

                if (entity_2)
                {
                    result = AI_FireWeapon_2(context, entity_2);
                    *(context + 0x3aa8) = result;
                    break;
                }
            }

            result_1 = result;
            result = *result;

            if (!result)
            {
                context_1[6] = 0;
                context_1[5] = 0;
                *context_1 = 0;
                return result;
            }
        }
    }

    context_1[6] = 0;
    context_1[5] = 0;
    *context_1 = 0;
    return result;
}
