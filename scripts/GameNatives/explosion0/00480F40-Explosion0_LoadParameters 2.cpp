// IGI native provenance: Explosion0_LoadParameters - 0x00480F40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00480F40-Explosion0_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Explosion0_LoadParameters.h"
unsigned int Explosion0_LoadParameters(int* context)

{
    int data_val = context[0x10];

    if (data_val)
        context[0x10] = data_val - 1;

    unsigned int result = context[0x10];

    if (!result)
    {
        result = context[0xe];

        if (result)
        {
            Weapon_1006011(context);
            int data_2 = context[0xe];
            context[0xe] = data_2 - 1;

            if (data_2 == 1)
                return QTask_FindType(context);

            result = Sys_FreeMemory_14(Level_SetMission_5(), context[0xf]);
            context[0x10] = result;
        }
    }

    return result;
}
