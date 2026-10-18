// IGI native provenance: Sys_CheckHeap_11 - 0x004B33A0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B33A0-Sys_CheckHeap_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B33A0-Sys_CheckHeap_11.hpp"
int Sys_CheckHeap_11(int context, float* arg2, float arg3, float arg4, float arg5)

{
    float item_50;
    Sys_ReadFileBlock_12(&item_50, arg3, arg4, arg5);
    float item_4c;
    float item_48;
    float item_28 = item_50 * *arg2 + item_48 * arg2[6] + item_4c * arg2[3];
    float item_24 = item_50 * arg2[1] + item_48 * arg2[7] + item_4c * arg2[4];
    float item_20 = item_50 * arg2[2] + item_48 * arg2[8] + item_4c * arg2[5];
    float item_44;
    float item_40;
    float item_3c;
    float item_1c = item_44 * *arg2 + item_3c * arg2[6] + item_40 * arg2[3];
    float item_18 = item_44 * arg2[1] + item_3c * arg2[7] + item_40 * arg2[4];
    float item_14 = item_44 * arg2[2] + item_3c * arg2[8] + item_40 * arg2[5];
    float item_38;
    float item_34;
    float item_30;
    float item_10 = item_38 * *arg2 + item_30 * arg2[6] + item_34 * arg2[3];
    float item_c = item_38 * arg2[1] + item_30 * arg2[7] + item_34 * arg2[4];
    int item_2c;
    int result = arg2[9] + item_2c + 1;
    int result_1 = result;
    float item_8 = item_38 * arg2[2] + item_30 * arg2[8] + item_34 * arg2[5];
    memcpy(context, &item_28, 0x28);
    return result;
}
