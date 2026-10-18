// IGI native provenance: SCameraControl_LoadParameters - 0x00440D70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00440D70-SCameraControl_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SCameraControl_LoadParameters.hpp"
char SCameraControl_LoadParameters(long double context , void* arg2)

{
    (*((Env_UpdateCameraLens_3() << 2) + &g_a970e0))(arg2);
    char data_4 = *(arg2 + 0x190);

    if (data_4)
        *(arg2 + 0x2a0) = 0;
    else
    {
        data_4 = Script_FormatString_6(arg2 + 0x2a4);

        if (data_4)
        {
            Script_SpawnNewTask_7(arg2 + 0x2a4, arg2);
            char item_8 = context;
            *(arg2 + 0x2a0) = item_8;
            return item_8;
        }
    }

    return data_4;
}
