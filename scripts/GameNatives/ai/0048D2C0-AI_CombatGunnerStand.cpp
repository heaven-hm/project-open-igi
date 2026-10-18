// IGI native provenance: AI_CombatGunnerStand - 0x0048D2C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048D2C0-AI_CombatGunnerStand.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatGunnerStand.hpp"

int* AI_CombatGunnerStand(void* self, void* args)

{
    void* dest_ptr = *(args + 0x28);
    *(args + 0x18) = Gunner_Combat;
    *(args + 0x20) = FX_SmokeEmit_19;
    *(self + 0x3dd4) = *(self + 0x3d44);
    *(self + 0x3dd0) = 0xffffffff;
    *(self + 0x3dd8) = 0xffffffff;
    int* i_1 = **(dest_ptr + 8);
    int* i;

    for (i = *i_1; i; i = *i)
    {
        int* counter_val_1 = i_1[0xb];

        if (counter_val_1)
        {
            void* counter_val_2 = *counter_val_1;

            if (counter_val_2)
            {
                i = AI_FireWeapon_2(self, counter_val_2);
                *(self + 0x3aa8) = i;
                break;
            }
        }

        i_1 = i;
    }

    *(dest_ptr + 0x1c) = 0x1e;
    *(dest_ptr + 8) = 0;
    *(dest_ptr + 0x20) = 0;
    return i;
}
