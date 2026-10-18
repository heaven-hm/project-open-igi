// IGI native provenance: FX_FireBallUpdate_7 - 0x004865B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004865B0-FX_FireBallUpdate_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004865B0-FX_FireBallUpdate_7.hpp"
int FX_FireBallUpdate_7(int context, int arg2, int arg3, float arg4)

{
    int i_1 = 3;
    int* source_val = context + arg2 * 0x102c + 0x30;
    int result;
    int i;

    do
    {
        *(*source_val + 4) = arg3;
        *(*source_val + 8) = arg4;
        Sys_CloseFileStream_18(*source_val);
        result = Sys_AllocateMemory_22(*source_val);
        source_val = &source_val[1];
        i = i_1;
        i_1 -= 1;
        arg4 = result + arg4;
    } while (i != 1);
    return result;
}
