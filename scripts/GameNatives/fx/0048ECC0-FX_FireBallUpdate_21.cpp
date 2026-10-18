// IGI native provenance: FX_FireBallUpdate_21 - 0x0048ECC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048ECC0-FX_FireBallUpdate_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048ECC0-FX_FireBallUpdate_21.hpp"
int FX_FireBallUpdate_21(int context, int* arg2)

{
    int* source_1 = arg2[0xa] + 8;
    void* data_2 = Table_GetAt(*(context + 0x14) + 0x4c4, 0);
    int item_24 = source_1[1];
    int item_28 = *source_1;
    char* item_2c = "Fire At Node - %d, %d";
    int item_30 = 0xffffffff;
    int item_34 = context;
    int data_4 = source_1[1];
    source_1[1] = data_4 - 1;

    if (data_4 < 0 || !data_2)
        return AI_CombatTarget_2(context, arg2);

    int item_8;
    Script_ParseEnumReal32_13(data_2, &item_8);
    int data_5 = item_8;

    if (!data_5)
    {
        data_5 = source_1[4];

        if (data_5)
        {
            *(context + 0x3c94) = 0x3f800000;
            return data_5;
        }

        Script_SpawnNewTask_13(data_2, 1);
        *(source_1 + 0x11) = 0;
    }

    Script_CallNativeHandler_23(context + 0x3e88, Script_ExecuteLevelScript_23(*(context + 0x3b6c), *source_1));
    char data_10;

    if (!*(source_1 + 0x11))
    {
        Combat_BeginBurst(context, 0);
        *(source_1 + 0x11) = 1;
        data_10 = *(source_1 + 0x11);
    }

    if (*(source_1 + 0x11) || data_10)
    {
        int data_11 = source_1[3];

        if (data_11 >= 0)
        {
            source_1[3] = data_11 - 1;
            return data_11 - 1;
        }

        data_10 = Combat_RunFire(context);

        if (data_10)
        {
            int entity_2 = source_1[2];
            *(source_1 + 0x11) = 0;
            source_1[3] = entity_2;
        }
    }

    return data_10;
}
