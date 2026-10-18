// IGI native provenance: ExplosionObj_InvokeTrailUpdate_43B490 - 0x0043B490 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043B490-ExplosionObj_InvokeTrailUpdate_43B490.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043B490-ExplosionObj_InvokeTrailUpdate_43B490.hpp"
int* ExplosionObj_InvokeTrailUpdate_43B490(void* arg1, int arg2)

{
    (*((FX_TrailUpdate() << 2) + &g_a96ae0))(arg1, arg2);
    *(arg1 + 0x238) = 0;
    *(arg1 + 0x23c) = 0;
    *(arg1 + 0x23d) = 0;
    int* result = Script_ExecuteLevelScript_6(arg1 + 0x240);
    *(arg1 + 0x290) = 0;
    *(arg1 + 0x2a0) = 0;
    *(arg1 + 0x2b0) = 0x41200000;
    *(arg1 + 0x2b4) = 1;
    *(arg1 + 0x2b8) = 0xffffffff;
    return result;
}
