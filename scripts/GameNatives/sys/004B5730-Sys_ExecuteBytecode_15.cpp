// IGI native provenance: Sys_ExecuteBytecode_15 - 0x004B5730 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B5730-Sys_ExecuteBytecode_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B5730-Sys_ExecuteBytecode_15.hpp"
int Sys_ExecuteBytecode_15(float context, float arg2, int arg3, float arg4, float arg5, int arg6, float arg7, float arg8, float arg9, float arg10, float arg11, float arg12, int arg13, int arg14, int arg15)

{
    float item_30 = arg7 * 0.0078125f;
    float item_2c = arg8 * 0.0078125f;
    int temp_2 = g_a84a54;
    float item_48;
    int* item_4c = &item_48;
    float item_28 = arg9 * 0.0078125f;
    int item_3c = 0;
    int item_38 = 0;
    int item_18 = 0;
    int item_14 = 0;
    int item_5c = 1;
    int item_50 = 2;
    item_48 = context - 0.5f;
    g_a84a50 = 0;
    g_a84a58 = 0;
    int result = (*((temp_2 << 2) + &g_a94e84))(&item_5c, item_5c, arg14, arg15, item_50,
        item_4c, item_48, arg2 - 0.5f, arg3, item_3c, item_38, arg13, item_30, item_2c, item_28,
        arg4 - 0.5f, arg5 - 0.5f, arg6, item_18, item_14, arg13, arg10 * 0.0078125f,
        arg11 * 0.0078125f, arg12 * 0.0078125f);
    return result;
}
