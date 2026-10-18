// IGI native provenance: FX_FireBallUpdate_2 - 0x00481360 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00481360-FX_FireBallUpdate_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00481360-FX_FireBallUpdate_2.hpp"
int FX_FireBallUpdate_2(void* context, float arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9)

{
    *(context + 0x1f8) = arg3;
    *(context + 0x1f0) = arg2;
    *(context + 0x1fc) = arg4;
    *(context + 0x204) = arg5;
    *(context + 0x200) = arg5;
    *(context + 0x208) = arg6;
    *(context + 0x20c) = arg7;
    int dest_val = arg9;
    *(context + 0x210) = arg8;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int result = ~i;
    int source_1;
    int dest_3;
    dest_3 = memcpy(context + 0x214, dest_val - result, result & 0xfffffffc);
    memcpy(dest_3, source_1, result & 3);
    return result;
}
