// IGI native provenance: Qsprite_Register4Az - 0x004B53B0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B53B0-Qsprite_Register4Az.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Qsprite_Register4Az.h"
int Qsprite_Register4Az(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, float arg10, float arg11, float arg12, int arg13, int arg14, int arg15, int arg16, int arg17)

// Sig: void Qsprite_Register4Az(/*14 args*/)
// Note: Registers a QUAD-SPRITE with full transform data: 14 args, float
// parameters scaled by constant 0x534AB0, writes sprite tables
// 0xA84A50/0xA84A58 (verified). Low-level renderer API - map arg layout from
// callers before use.
{
    int* data_4 = context[2] + arg15 * 0x24;
    long double fpu_val_7_1 = arg10 * 0.0078125f;
    int temp_3 = data_4[5];
    int temp_4 = data_4[6];
    int temp_5 = data_4[7];
    int temp_6 = data_4[6];
    int temp_7 = data_4[5];
    int temp_8 = data_4[8];
    int temp_9 = data_4[7];
    int temp_10 = data_4[8];
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
    g_a84a50 = *data_4;
    g_a84a58 = *context;
    int result = (*((g_a84a54 << 2) + &g_a94e84))(&item_a4, item_a4, arg16, arg17, item_98,
        item_94, item_90, arg3, arg13, temp_3, temp_4, arg14, item_78, item_74, item_70, arg4, arg5,
        arg13, temp_5, temp_6, arg14, item_54, item_50, item_4c, arg6, arg7, arg13, temp_7, temp_8, arg14,
        item_30, item_2c, item_28, arg8, arg9, arg13, temp_9, temp_10, arg14, item_c, item_8, item_4);
    return result;
}
