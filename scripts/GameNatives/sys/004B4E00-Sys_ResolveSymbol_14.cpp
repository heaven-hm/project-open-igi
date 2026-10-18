// IGI native provenance: Sys_ResolveSymbol_14 - 0x004B4E00 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4E00-Sys_ResolveSymbol_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B4E00-Sys_ResolveSymbol_14.hpp"
int Sys_ResolveSymbol_14(int* context, int arg2, float arg3, float arg4, float arg5, float arg6, int arg7, int arg8, int arg9)

{
    long double fpu_val_7_1 = arg4 * 0.0078125f;
    int* data_2 = context[2] + arg7 * 0x24;
    int item_34 = 0x3f800000;
    int item_10 = 0x3f800000;
    int item_40 = 0x3f800000;
    int item_1c = 0x3f800000;
    int temp_1 = data_2[5];
    int temp_2 = data_2[6];
    int temp_3 = data_2[7];
    int temp_4 = data_2[8];
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg5 * 0.0078125f;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    long double fpu_val_7_5 = arg6 * 0.0078125f;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    float item_48 = data_2[2] + arg2;
    float item_44 = *(data_2 + 0xa) + arg3;
    float item_24 = data_2[3] + data_2[2] + arg2;
    long double fpu_val_7_14 = *(data_2 + 0xe) + *(data_2 + 0xa);
    float* item_4c = &item_48;
    int item_5c = 0;
    int item_50 = 2;
    float item_20 = fpu_val_7_14 + arg3;
    g_a84a50 = *data_2;
    int data_4 = g_a84a54;
    g_a84a58 = *context;
    return (*((data_4 << 2) + &g_a94e84))(&item_5c, item_5c, arg8, arg9, item_50, item_4c, item_48,
        item_44, item_40, temp_1, temp_2, item_34, item_30, item_2c, item_28, item_24, item_20, item_1c,
        temp_3, temp_4, item_10, item_c, item_8, item_4);
}
