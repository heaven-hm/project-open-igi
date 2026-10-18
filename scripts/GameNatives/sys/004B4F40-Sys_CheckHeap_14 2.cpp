// IGI native provenance: Sys_CheckHeap_14 - 0x004B4F40 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4F40-Sys_CheckHeap_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_CheckHeap_14.h"
int Sys_CheckHeap_14(int* context, float arg2, float arg3, float arg4, float arg5, float arg6, int arg7, int arg8, int arg9, int arg10)

{
    int item_34 = 0x3f800000;
    int item_10 = 0x3f800000;
    int* data_3 = context[2] + arg7 * 0x24;
    long double fpu_val_7_1 = arg4 * 0.0078125f;
    int temp_1 = data_3[5];
    int temp_2 = data_3[6];
    int temp_3 = data_3[7];
    int temp_4 = data_3[8];
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg5 * 0.0078125f;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    long double fpu_val_7_5 = arg6 * 0.0078125f;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    float item_48 = data_3[2] + arg2;
    float item_44 = *(data_3 + 0xa) + arg3;
    float item_24 = data_3[3] + data_3[2] + arg2;
    long double fpu_val_7_14 = *(data_3 + 0xe) + *(data_3 + 0xa);
    float* item_4c = &item_48;
    int item_5c = 0;
    int item_50 = 2;
    float item_20 = fpu_val_7_14 + arg3;
    g_a84a50 = *data_3;
    int data_5 = g_a84a54;
    g_a84a58 = *context;
    return (*((data_5 << 2) + &g_a94e84))(&item_5c, item_5c, arg8, arg9, item_50, item_4c, item_48,
        item_44, arg10, temp_1, temp_2, item_34, item_30, item_2c, item_28, item_24, item_20, arg10, temp_3,
        temp_4, item_10, item_c, item_8, item_4);
}
