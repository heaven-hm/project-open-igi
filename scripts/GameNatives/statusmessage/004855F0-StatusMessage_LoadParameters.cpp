// IGI native provenance: StatusMessage_LoadParameters - 0x004855F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004855F0-StatusMessage_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004855F0-StatusMessage_LoadParameters.hpp"
int* StatusMessage_LoadParameters(long double context , int* arg2)

{
    int* result;
    result = *(arg2 + 0x1d6);

    if (result)
        arg2[0x74] += 1;

    result = arg2[0x71];

    if (!result)
    {
    label_485634:
        result = arg2[0x72] + 0xd2;

        if (*(g_bc2360 + 0x68) > result)
        {
            if (*(arg2 + 0x1c5) && arg2[0x75])
                return QTask_FindType(arg2);

            result = Script_FormatString_6(&arg2[0x18]);

            if (result)
            {
            label_485682:

                if (*(arg2 + 0x1c5))
                {
                label_4856aa:
                    FX_RmS(*(StatusScreen_Get() + 0x4e4), arg2);
                    result = g_bc2360;
                    int entity_2 = result[0x1a];
                    arg2[0x75] = 1;
                    arg2[0x72] = entity_2;
                }
                else
                {
                    result = Script_SpawnNewTask_7(&arg2[0x18], arg2);
                    long double f0_1 = 0.0;
                    context - f0_1;
                    result = (context < f0_1 ? 1 : 0) << 8 | (FCMP_UO(context, f0_1) ? 1 : 0) << 0xa
                        | (context == f0_1 ? 1 : 0) << 0xe | 0x800;

                    if (!(*(&result + 1) & 0x40))
                        goto label_4856aa;
                }
            }
            else
            {
                result = *(arg2 + 0x1c5);

                if (result)
                    goto label_485682;
            }
        }
    }
    else
    {
        result = arg2[0x75];

        if (!result)
            goto label_485634;

        result = *(arg2 + 0x1c5);

        if (result)
            goto label_485634;
    }

    return result;
}
