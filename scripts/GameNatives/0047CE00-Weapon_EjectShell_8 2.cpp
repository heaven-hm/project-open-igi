// IGI native provenance: Weapon_EjectShell_8 - 0x0047CE00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047CE00-Weapon_EjectShell_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Weapon_EjectShell_8.h"
int* Weapon_EjectShell_8(int* context, int arg2, int arg3)

{
    int temp_val = 0;
    int data_val = *context;
    int* entity_1;

    if (data_val > 0)
    {
        entity_1 = &context[1];

        while (*entity_1 != arg2)
        {
            temp_val += 1;
            entity_1 = &entity_1[3];

            if (temp_val >= data_val)
                goto label_47ce28;
        }
    }

    int* result;

    if (data_val <= 0 || !entity_1)
    {
    label_47ce28:
        entity_1 = &context[data_val * 3 + 1];
        result = &entity_1[1];
        *entity_1 = arg2;
        *result = arg3;
        *context += 1;
    }
    else
    {
        result = &entity_1[1];
        *result = entity_1[1] + arg3;
    }

    int source_1 = entity_1[2];

    if (source_1 != 0xffffffff)
    {
        int temp_4 = *result;

        if (temp_4 < 0)
        {
            *result = 0;
            return result;
        }

        if (temp_4 > source_1)
            *result = entity_1[2];
    }

    return result;
}
