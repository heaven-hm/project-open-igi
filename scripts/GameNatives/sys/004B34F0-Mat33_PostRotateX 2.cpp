// IGI native provenance: Mat33_PostRotateX - 0x004B34F0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B34F0-Sys_AllocateMemory_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_AllocateMemory_12.h"
int Mat33_PostRotateX(int context, float* arg2, float arg3)

{
    float item_50;
    Sys_CloseFileStream_12(&item_50, arg3);
    float item_44;
    float item_38;
    float item_28 = item_50 * *arg2 + item_38 * arg2[2] + item_44 * arg2[1];
    float item_4c;
    float item_40;
    float item_34;
    float item_24 = item_4c * *arg2 + item_34 * arg2[2] + item_40 * arg2[1];
    float item_48;
    float item_3c;
    float item_30;
    float item_20 = item_48 * *arg2 + item_30 * arg2[2] + item_3c * arg2[1];
    float item_1c = item_50 * arg2[3] + item_38 * arg2[5] + item_44 * arg2[4];
    float item_18 = item_4c * arg2[3] + item_34 * arg2[5] + item_40 * arg2[4];
    float item_14 = item_48 * arg2[3] + item_30 * arg2[5] + item_3c * arg2[4];
    float item_10 = item_50 * arg2[6] + item_38 * arg2[8] + item_44 * arg2[7];
    float item_c = item_4c * arg2[6] + item_34 * arg2[8] + item_40 * arg2[7];
    int result;
    int item_4 = arg2[9] + result + 1;
    float item_8 = item_48 * arg2[6] + item_30 * arg2[8] + item_3c * arg2[7];
    memcpy(context, &item_28, 0x28);
    return result;
}
