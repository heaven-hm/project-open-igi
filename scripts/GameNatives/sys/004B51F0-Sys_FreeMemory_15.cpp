// IGI native provenance: Sys_FreeMemory_15 - 0x004B51F0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B51F0-Sys_FreeMemory_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B51F0-Sys_FreeMemory_15.hpp"
int Sys_FreeMemory_15(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, float arg10, float arg11, float arg12, int arg13, int arg14, int arg15)

{
    int item_7c = 0x3f800000;
    int item_58 = 0x3f800000;
    int item_34 = 0x3f800000;
    int item_10 = 0x3f800000;
    int* data_2 = context[2] + arg13 * 0x24;
    int item_88 = 0x3f800000;
    int item_64 = 0x3f800000;
    int item_40 = 0x3f800000;
    int item_1c = 0x3f800000;
    int temp_1 = data_2[5];
    int temp_2 = data_2[6];
    int temp_3 = data_2[7];
    int temp_4 = data_2[6];
    int temp_5 = data_2[5];
    int temp_6 = data_2[8];
    int temp_7 = data_2[7];
    int temp_8 = data_2[8];
    long double fpu_val_7_1 = arg10 * 0.0078125f;
    int item_90 = arg2;
    float item_78 = fpu_val_7_1;
    float item_54 = fpu_val_7_1;
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg11 * 0.0078125f;
    float item_74 = fpu_val_7_3;
    float item_50 = fpu_val_7_3;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    long double fpu_val_7_5 = arg12 * 0.0078125f;
    int* item_94 = &item_90;
    float item_70 = fpu_val_7_5;
    float item_4c = fpu_val_7_5;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    int item_a4 = 0;
    int item_98 = 4;
    g_a84a50 = *data_2;
    int data_4 = g_a84a54;
    g_a84a58 = *context;
    int result = (*((data_4 << 2) + &g_a94e84))(&item_a4, item_a4, arg14, arg15, item_98,
        item_94, item_90, arg3, item_88, temp_1, temp_2, item_7c, item_78, item_74, item_70, arg4, arg5,
        item_64, temp_3, temp_4, item_58, item_54, item_50, item_4c, arg6, arg7, item_40, temp_5, temp_6,
        item_34, item_30, item_2c, item_28, arg8, arg9, item_1c, temp_7, temp_8, item_10, item_c, item_8,
        item_4);
    return result;
}
