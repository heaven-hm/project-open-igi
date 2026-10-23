// IGI native provenance: DefineHumanPlayerWeaponCategory - 0x00410240 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00410240-Define_HumanPlayerWeaponCategory.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00410240-Define_HumanPlayerWeaponCategory.hpp"
int DefineHumanPlayerWeaponCategory(int context, float arg2 )

// Sig: void DefineHumanPlayerWeaponCategory(...)
// Note: Registers one weapon-category entry for the human player during a
// humanplayer definition block (argc unknown - parser context). Pairs with
// DefineHumanPlayerWeaponCycle/Cycle ordering.
{
    int* i = &g_56e0a4;
    int source_val = 0;
    int* i_1 = &g_56e0a4;

    do
    {
        int data_1 = source_val;
        source_val += 1;
        int item_30 = data_1;
        int j_2 = Script_GetArg(arg2, item_30);
        i[-1] = j_2;

        if (j_2 > 0)
        {
            int j_1 = j_2;
            int j;

            do
            {
                int data_2 = source_val;
                source_val += 1;
                item_30 = data_2;
                *i = Script_GetArg(arg2, item_30);
                i = &i[1];
                j = j_1;
                j_1 -= 1;
            } while (j != 1);
            i = i_1;
        }

        i = &i[9];
        i_1 = i;
    } while (i < &g_56e1e8);

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
