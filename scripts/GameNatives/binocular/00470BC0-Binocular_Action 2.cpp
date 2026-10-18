// IGI native provenance: Binocular_Action - 0x00470BC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00470BC0-Binocular_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Binocular_Action.h"
int* Binocular_Action(void* context, int arg2)

{
    int* result = *(context + 0x1b0);

    if (result)
    {
        Weapon_UpdateMuzzleFlash_2(**(context + 0x1c0), context + 0x150);
        result = Weapon_UpdateBarrel_2(**(context + 0x1c0), context + 0x168);
        int* i = *(context + 8);

        if (*i && i)
        {
            int entity_3 = g_afa7e0;

            do
            {
                int* temp_3 = *i;
                void* data_5;

                if (!temp_3)
                    data_5 = nullptr;
                else
                {
                    int data_2 = *temp_3;
                    int data_3 = -(data_2);
                    data_5 = (data_3 - data_3) & temp_3;
                }

                *((entity_3 << 2) + &g_afa6e0) = data_5;
                g_afa7e0 = entity_3 + 1;
                unsigned int data_7 = FX_DebrisSpawn_8();
                int entity_5 = 0;
                entity_5 = i[7];
                result = *(((data_7 * 0x180 + entity_5) << 2) + &g_a96ae0);

                if (result)
                    result = result(i, arg2);

                entity_3 = g_afa7e0 - 1;
                g_afa7e0 = entity_3;
                i = *((entity_3 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    return result;
}
