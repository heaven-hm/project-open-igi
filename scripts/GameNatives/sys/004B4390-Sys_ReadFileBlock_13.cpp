// IGI native provenance: Sys_ReadFileBlock_13 - 0x004B4390 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4390-Sys_ReadFileBlock_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B4390-Sys_ReadFileBlock_13.hpp"
int Sys_ReadFileBlock_13(int context, float arg2)

{
    float* data_val = arg2;
    long double fpu_val_7_1 = data_val[3] * data_val[7];
    long double fpu_val_6_1 = data_val[6] * data_val[5];
    long double fpu_val_5_1 = data_val[8] * data_val[4];
    long double fpu_val_4_1 = data_val[5] * data_val[7];
    float item_30 = data_val[8] * data_val[3];
    float item_2c = data_val[6] * data_val[4];
    long double fpu_val_3_9 =
        fpu_val_5_1 * *data_val + fpu_val_7_1 * data_val[2] + fpu_val_6_1 * data_val[1] - fpu_val_4_1 * *data_val - item_30 * data_val[1];
    arg2 = 1.0f / (fpu_val_3_9 - item_2c * data_val[2]);
    float item_28 = (fpu_val_5_1 - fpu_val_4_1) * arg2;
    float item_24 = -(((item_30 - fpu_val_6_1) * arg2));
    float item_20 = (fpu_val_7_1 - item_2c) * arg2;
    float item_1c = -(((data_val[8] * data_val[1] - data_val[2] * data_val[7]) * arg2));
    float item_18 = (data_val[8] * *data_val - data_val[6] * data_val[2]) * arg2;
    float item_14 = -(((*data_val * data_val[7] - data_val[6] * data_val[1]) * arg2));
    float item_10 = (data_val[5] * data_val[1] - data_val[2] * data_val[4]) * arg2;
    float item_c = -(((data_val[5] * *data_val - data_val[3] * data_val[2]) * arg2));
    int result = data_val[9] + 1;
    int result_1 = result;
    float item_8 = (data_val[4] * *data_val - data_val[3] * data_val[1]) * arg2;
    memcpy(context, &item_28, 0x28);
    return result;
}
