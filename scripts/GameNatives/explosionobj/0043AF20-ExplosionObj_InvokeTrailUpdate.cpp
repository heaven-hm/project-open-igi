// IGI native provenance: ExplosionObj_InvokeTrailUpdate - 0x0043AF20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043AF20-ExplosionObj_InvokeTrailUpdate.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043AF20-ExplosionObj_InvokeTrailUpdate.hpp"
char* ExplosionObj_InvokeTrailUpdate(void* arg1, int arg2)

{
    (*((FX_TrailUpdate() << 2) + &g_a96ae0))(arg1, arg2);
    *(arg1 + 0x238) = 0;
    *(arg1 + 0x23c) = 0;
    *(arg1 + 0x240) = 0;
    *(arg1 + 0x241) = 0;
    Script_ExecuteLevelScript_6(arg1 + 0x244);
    char* result = arg1 + 0x294;
    int i_1 = 4;
    int i;

    do
    {
        *result = 0;
        result = &result[0x10];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    *(arg1 + 0x2d4) = 0;
    return result;
}
