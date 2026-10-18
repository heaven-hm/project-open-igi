// IGI native provenance: Sys_DispatchOpcode_14 - 0x004B4AF0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4AF0-Sys_DispatchOpcode_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_DispatchOpcode_14.h"
int Sys_DispatchOpcode_14(float context, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, int arg8, int arg9, int arg10, int arg11)

{
    long double fpu_val_7_1 = arg5 * 0.0078125f;
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg6 * 0.0078125f;
    float item_2c = fpu_val_7_3;
    float item_8 = fpu_val_7_3;
    long double fpu_val_7_5 = arg7 * 0.0078125f;
    float item_48 = context;
    int item_5c = 0;
    int* item_4c = &item_48;
    float item_28 = fpu_val_7_5;
    float item_4 = fpu_val_7_5;
    g_a84a50 = 0;
    g_a84a58 = 0;
    int result = (*((g_a84a54 << 2) + &g_a94e84))(&item_5c, item_5c, arg10, arg11, 2,
        item_4c, item_48, arg2, arg9, 0, 0, arg8, item_30, item_2c, item_28, context + arg3, arg2 + arg4,
        arg9, 0x3f800000, 0x3f800000, arg8, item_c, item_8, item_4);
    return result;
}
