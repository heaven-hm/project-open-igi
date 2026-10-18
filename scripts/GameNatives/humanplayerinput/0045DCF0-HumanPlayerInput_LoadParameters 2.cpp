// IGI native provenance: HumanPlayerInput_LoadParameters - 0x0045DCF0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045DCF0-HumanPlayerInput_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayerInput_LoadParameters.h"
int HumanPlayerInput_LoadParameters(long double context , void* arg2)

{
    void* flagP = *(arg2 + 0x14);
    void item_a0;

    if (!*(arg2 + 0x81))
    {
    label_45dd48:
        void* item_c4_1 = arg2;
        AI_CheckSector_16(&item_a0);

        if (*(arg2 + 0x80))
        {
            int data_5 = *(arg2 + 0x18c);

            if (data_5 < 0x2328)
            {
                memcpy(data_5 * 0xa0 + *(arg2 + 0x188), &item_a0, 0xa0);
                int data_7 = *(arg2 + 0x190) + 1;
                *(arg2 + 0x18c) += 1;
                *(arg2 + 0x190) = data_7;
            }
        }
    }
    else
    {
        int entity_1 = *(arg2 + 0x190);

        if (!entity_1)
            goto label_45dd48;

        int data_1 = *(arg2 + 0x18c);

        if (data_1 >= entity_1)
            goto label_45dd48;

        memcpy(&item_a0, data_1 * 0xa0 + *(arg2 + 0x184), 0xa0);
        *(arg2 + 0x18c) = data_1 + 1;
    }

    if (!Script_FormatString_6(arg2 + 0x194))
        AI_FireWeapon_29(1);
    else
    {
        Script_SpawnNewTask_7(arg2 + 0x194, arg2);
        AI_FireWeapon_29(context);
    }

    int item_b0 = 0xffffffff;
    void* item_ac = &item_a0;
    unsigned int data_11 = Script_ParseEnumReal32_15();
    int data_12 = 0;
    data_12 = *(flagP + 0x1c);
    int result = *(((data_11 * 0x180 + data_12) << 2) + &g_a96ae0);

    if (!result)
        return result;

    return result(flagP, &item_b0);
}
