// IGI native provenance: Guard_CombatStart - 0x0048D000 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048D000-Guard_CombatStart.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Guard_CombatStart.hpp"
int* Guard_CombatStart(void* context, void* arg2)

{
    void* dest_val = *(arg2 + 0x28);
    *(arg2 + 0x18) = Guard_CombatStand;
    *(arg2 + 0x20) = FX_SmokeEmit_19;
    int entity_val = *(context + 0x3d44);
    *(context + 0x3dd0) = 0xffffffff;
    *(context + 0x3dd4) = entity_val;
    *(context + 0x3dd8) = 0xffffffff;
    int** result_1 = **(dest_val + 8);
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
                *(dest_val + 8) = 0;
                *(dest_val + 0x20) = 0;
                return result;
            }
        }
    }

    *(dest_val + 8) = 0;
    *(dest_val + 0x20) = 0;
    return result;
}
