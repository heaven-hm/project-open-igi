// IGI native provenance: GunShot_Setup - 0x0047B690 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047B690-GunShot_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunShot_Setup.hpp"
int GunShot_Setup(int* context)

{
    if (context[0x25])
    {
        int data_1 = context[0x1b];
        int entity_1 = context[0x1c];
        int* item_c = context;
        int item_8_1 = data_1;
        int item_4_1 = entity_1;
        int i = 0;

        if (*AI_CombatTarget_18() > 0)
        {
            int source_1 = 8;
            int* data_9;

            do
            {
                int data_4 = 0;
                data_4 = *(*(AI_CombatTarget_18() + source_1) + 0x1c);
                unsigned int data_6 = AI_UpdateSensors_17();
                (*(((data_4 + data_6 * 0x180) << 2) + &g_a96ae0))(*(AI_CombatTarget_18() + source_1), &item_c);
                i += 1;
                source_1 += 4;
                data_9 = AI_CombatTarget_18();
            } while (i < *data_9);
        }
    }

    Gun_CastRound(context);
    Weapon_EjectShell_7(context);
    int result = context[0x25];

    if (!result)
    {
        result = context[0x1a];

        if (result)
        {
            result = context[0x27];

            if (!result)
                return QTask_FindType(context);
        }
    }

    return result;
}
