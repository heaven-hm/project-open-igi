// IGI native provenance: AI_CombatRPGStand - 0x0048CB40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048CB40-AI_CombatRPGStand.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatRPGStand.h"

int* AI_CombatRPGStand(void* self, void* args)

{
    void* base_ptr = *(args + 0x28);

    if (!AI_MoveToNode_19(*(self + 0x14)))
        *(args + 0x18) = RPG_CombatStand;
    else
        *(args + 0x18) = RPG_CombatKneel;

    *(args + 0x20) = FX_SmokeEmit_19;
    *(self + 0x3dd4) = *(self + 0x3d44);
    *(self + 0x3dd0) = 0xffffffff;
    *(self + 0x3dd8) = 0xffffffff;
    int** result_1 = **(base_ptr + 8);
    int* result = *result_1;

    if (result)
    {
        while (true)
        {
            int* counter_val_1 = result_1[0xb];

            if (counter_val_1)
            {
                void* counter_val_2 = *counter_val_1;

                if (counter_val_2)
                {
                    result = AI_FireWeapon_2(self, counter_val_2);
                    *(self + 0x3aa8) = result;
                    break;
                }
            }

            result_1 = result;
            result = *result;

            if (!result)
            {
                *(base_ptr + 8) = 0;
                return result;
            }
        }
    }

    *(base_ptr + 8) = 0;
    return result;
}
