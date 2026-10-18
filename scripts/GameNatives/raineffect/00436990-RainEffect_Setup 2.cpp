// IGI native provenance: RainEffect_Setup - 0x00436990 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00436990-RainEffect_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "RainEffect_Setup.h"
int RainEffect_Setup(long double context , void* arg2)

{
    int result = *(arg2 + 0x20);

    if (!result)
        return result;

    if (Script_FormatString_6(arg2 + 0x38))
    {
        Script_SpawnNewTask_7(arg2 + 0x38, arg2);
        Display_RestoreDevice_17(*(arg2 + 0x20), context);
    }

    return Display_ClearSurfaces_17(*(arg2 + 0x20));
}
