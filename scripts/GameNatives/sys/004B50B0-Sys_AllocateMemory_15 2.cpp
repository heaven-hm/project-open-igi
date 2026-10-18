// IGI native provenance: Sys_AllocateMemory_15 - 0x004B50B0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B50B0-Sys_AllocateMemory_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_AllocateMemory_15.h"
int Sys_AllocateMemory_15(int* context, float arg2, float arg3, float arg4, float arg5, float arg6, int arg7, int arg8, int arg9, int arg10, int arg11)

{
    long double fpu_val_7_1 = arg4 * 0.0078125f;
    int* data_4 = context[2] + arg9 * 0x24;
    int temp_2 = data_4[5];
    int temp_3 = data_4[6];
    int temp_4 = data_4[7];
    int temp_5 = data_4[8];
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg5 * 0.0078125f;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    long double fpu_val_7_5 = arg6 * 0.0078125f;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    float item_48 = data_4[2] + arg2;
    float item_44 = *(data_4 + 0xa) + arg3;
    float item_24 = data_4[3] + data_4[2] + arg2;
    long double fpu_val_7_14 = *(data_4 + 0xe) + *(data_4 + 0xa);
    float* item_4c = &item_48;
    int item_5c = 0;
    int item_50 = 2;
    float item_20 = fpu_val_7_14 + arg3;
    g_a84a50 = *data_4;
    int data_6 = g_a84a54;
    g_a84a58 = *context;
    return (*((data_6 << 2) + &g_a94e84))(&item_5c, item_5c, arg10, arg11, item_50, item_4c, item_48,
        item_44, arg8, temp_2, temp_3, arg7, item_30, item_2c, item_28, item_24, item_20, arg8, temp_4,
        temp_5, arg7, item_c, item_8, item_4);
}
