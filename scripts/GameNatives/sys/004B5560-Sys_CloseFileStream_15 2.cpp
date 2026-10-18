// IGI native provenance: Sys_CloseFileStream_15 - 0x004B5560 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B5560-Sys_CloseFileStream_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_CloseFileStream_15.h"
int Sys_CloseFileStream_15(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12, int arg13, int arg14, int arg15, int arg16, int arg17, float arg18, float arg19, float arg20, int arg21, int arg22, int arg23, int arg24, int a25)

{
    int item_a4 = 0;
    long double fpu_val_7_1 = arg18 * 0.0078125f;
    float item_78 = fpu_val_7_1;
    float item_54 = fpu_val_7_1;
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg19 * 0.0078125f;
    float item_74 = fpu_val_7_3;
    float item_50 = fpu_val_7_3;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    int item_90 = arg2;
    long double fpu_val_7_5 = arg20 * 0.0078125f;
    int* item_94 = &item_90;
    float item_70 = fpu_val_7_5;
    float item_4c = fpu_val_7_5;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    int item_98 = 4;
    g_a84a50 = *(context[2] + arg23 * 0x24);
    g_a84a58 = *context;
    int result = (*((g_a84a54 << 2) + &g_a94e84))(&item_a4, item_a4, arg24, a25, item_98,
        item_94, item_90, arg3, arg21, arg10, arg11, arg22, item_78, item_74, item_70, arg4, arg5,
        arg21, arg12, arg13, arg22, item_54, item_50, item_4c, arg6, arg7, arg21, arg14, arg15, arg22,
        item_30, item_2c, item_28, arg8, arg9, arg21, arg16, arg17, arg22, item_c, item_8, item_4);
    return result;
}
