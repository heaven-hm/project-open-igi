// IGI native provenance: Sys_ReadFileBlock_14 - 0x004B4820 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B4820-Sys_ReadFileBlock_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B4820-Sys_ReadFileBlock_14.hpp"
int Sys_ReadFileBlock_14(float context, float arg2, float arg3, float arg4, float arg5, float arg6, float arg7, int arg8, int arg9)

{
    long double fpu_val_7_1 = arg5 * 0.0078125f;
    float item_48 = context;
    float item_30 = fpu_val_7_1;
    float item_c = fpu_val_7_1;
    long double fpu_val_7_3 = arg6 * 0.0078125f;
    int item_5c = 0;
    g_a84a50 = 0;
    g_a84a58 = 0;
    long double fpu_val_7_5 = arg7 * 0.0078125f;
    int result = (*((g_a84a54 << 2) + &g_a94e84))(&item_5c, item_5c, arg8, arg9, 2,
        &item_48, item_48, arg2, 0x3f800000, 0, 0, 0x3f800000, item_30, fpu_val_7_3, fpu_val_7_5,
        context + arg3, arg2 + arg4, 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000, item_c, fpu_val_7_3,
        fpu_val_7_5);
    return result;
}
