// IGI native provenance: Sys_PushStack_14 - 0x004B4BE0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4BE0-Sys_PushStack_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_PushStack_14.h"
int Sys_PushStack_14(float context, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, float arg13, float arg14, float arg15, float arg16, int arg17, int arg18, int arg19, int arg20, int arg21, int arg22, int arg23)

{
    float item_78 = arg5 * 0.0078125f;
    float item_54 = arg8 * 0.0078125f;
    float item_30 = arg11 * 0.0078125f;
    float item_90 = context;
    float item_c = arg14 * 0.0078125f;
    int item_84 = 0;
    int item_80 = 0;
    int item_60 = 0x3f800000;
    int item_5c = 0;
    int item_3c = 0;
    int item_38 = 0x3f800000;
    float item_74 = arg6 * 0.0078125f;
    int item_18 = 0x3f800000;
    int item_14 = 0x3f800000;
    int* item_94 = &item_90;
    float item_50 = arg9 * 0.0078125f;
    float item_2c = arg12 * 0.0078125f;
    float item_8 = arg15 * 0.0078125f;
    float item_70 = arg7 * 0.0078125f;
    float item_4c = arg10 * 0.0078125f;
    float item_28 = arg13 * 0.0078125f;
    float item_4 = arg16 * 0.0078125f;
    long double fpu_val_7_25 = context + arg3;
    float item_6c = fpu_val_7_25;
    long double fpu_val_6_1 = arg2 + arg4;
    float item_a8 = fpu_val_6_1;
    float item_44 = fpu_val_6_1;
    float item_24 = fpu_val_7_25;
    int item_a4 = 0;
    g_a84a50 = 0;
    g_a84a58 = 0;
    int result = (*((g_a84a54 << 2) + &g_a94e84))(&item_a4, item_a8, item_a4, arg22, arg23,
        4, item_94, item_90, arg2, arg21, item_84, item_80, arg17, item_78, item_74, item_70, item_6c,
        arg2, arg21, item_60, item_5c, arg18, item_54, item_50, item_4c, context, item_44, arg21, item_3c,
        item_38, arg19, item_30, item_2c, item_28, item_24, item_a8, arg21, item_18, item_14, arg20, item_c,
        item_8, item_4);
    return result;
}
