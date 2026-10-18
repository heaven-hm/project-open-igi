// IGI native provenance: AIAction_Combat - 0x0044D160 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044D160-AIAction_Combat.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044D160-AIAction_Combat.hpp"
int AIAction_Combat(int context, float arg2 )

// Sig: void AIAction_Combat(int args)
// Note: Orders the AI into COMBAT behaviour vs its current enemy. Param: args
// = combat argument from the calling script (retail guard scripts pass their
// combat target/context value). Use in AIEVENT_COMBAT branches.
{
    int* data_val = AI_CheckVisibility_2(g_5bda58);
    int data_1 = Script_GetArg(arg2, 0);
    int temp_val = g_5bda60;
    data_val[0xa] = 1;
    data_val[2] = temp_val;
    *(data_val + 0x29) = 0;
    data_val[9] = 0;
    int entity_2 = *(g_5bda58 + 0x3d38);

    if (entity_2 <= 0x1c)
    {
        int temp_1 = 0;
        temp_1 = lookup_table_44d23c[entity_2];

        switch (temp_1)
        {
            case 0:
            {
                AI_SelectTarget(sub_48cb40, data_val, data_1);
                break;
            }
            case 1:
            {
                AI_SelectTarget(sub_48d2c0, data_val, data_1);
                break;
            }
            case 2:
            {
                AI_SelectTarget(Guard_CombatStart, data_val, data_1);
                break;
            }
            case 3:
            {
                AI_SelectTarget(FX_ExplosionSpawn_18, data_val, data_1);
                break;
            }
            case 4:
            {
                AI_SelectTarget(Patrol_CombatStart, data_val, data_1);
                break;
            }
        }
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
