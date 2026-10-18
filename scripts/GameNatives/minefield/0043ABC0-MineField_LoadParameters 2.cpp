// IGI native provenance: MineField_LoadParameters - 0x0043ABC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043ABC0-MineField_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MineField_LoadParameters.h"
int MineField_LoadParameters(long double context , void* arg2)

{
    int result = *(arg2 + 0xcc);

    if (result != 0xffffffff)
    {
        if (result)
        {
            *(arg2 + 0xcc) = result - 1;
            return result - 1;
        }

        result = Script_FormatString_6(arg2 + 0x38);

        if (result)
        {
            result = Script_SpawnNewTask_7(arg2 + 0x38, arg2);
            long double f0_1 = 0.0;
            context - f0_1;
            result = (context < f0_1 ? 1 : 0) << 8 | (FCMP_UO(context, f0_1) ? 1 : 0) << 0xa
                | (context == f0_1 ? 1 : 0) << 0xe | 0x800;

            if (!(*(&result + 1) & 0x40))
            {
                Vehicle_1009011_2(arg2);
                result = *(arg2 + 0xc4) * 30.0f;
                *(arg2 + 0xcc) = result;
            }
        }
    }

    return result;
}
