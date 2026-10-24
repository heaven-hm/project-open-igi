// Native: Qsprite_Register4Az
// Retail VA: 0x4b53b0
// Source: Binary Ninja MCP decompile
// Signature: int32_t(int32_t* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9, float arg10, float arg11, float arg12, int32_t arg13, int32_t arg14, int32_t arg15, int32_t arg16, int32_t arg17)

  int32_t Qsprite_Register4Az(int32_t* arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9, float arg10, float arg11, float arg12, int32_t arg13, int32_t arg14, int32_t arg15, int32_t arg16, int32_t arg17)

// Sig: void Qsprite_Register4Az(/*14 args*/)
// Note: Registers a QUAD-SPRITE with full transform data: 14 args, float
// parameters scaled by constant 0x534AB0, writes sprite tables
// 0xA84A50/0xA84A58 (verified). Low-level renderer API - map arg layout from
// callers before use.
{
    int32_t* eax_4 = arg1[2] + arg15 * 0x24;
    long double x87_r7_1 = arg10 * 0.0078125f;
    int32_t edx_3 = eax_4[5];
    int32_t edx_4 = eax_4[6];
    int32_t edx_5 = eax_4[7];
    int32_t edx_6 = eax_4[6];
    int32_t edx_7 = eax_4[5];
    int32_t edx_8 = eax_4[8];
    int32_t edx_9 = eax_4[7];
    int32_t edx_10 = eax_4[8];
    int32_t var_90 = arg2;
    float var_78 = x87_r7_1;
    float var_54 = x87_r7_1;
    float var_30 = x87_r7_1;
    float var_c = x87_r7_1;
    long double x87_r7_3 = arg11 * 0.0078125f;
    float var_74 = x87_r7_3;
    float var_50 = x87_r7_3;
    float var_2c = x87_r7_3;
    float var_8 = x87_r7_3;
    long double x87_r7_5 = arg12 * 0.0078125f;
    int32_t* var_94 = &var_90;
    float var_70 = x87_r7_5;
    float var_4c = x87_r7_5;
    float var_28 = x87_r7_5;
    float var_4 = x87_r7_5;
    int32_t var_a4 = 0;
    int32_t var_98 = 4;
    g_a84a50 = *eax_4;
    g_a84a58 = *arg1;
    int32_t result = (*((g_a84a54 << 2) + &g_a94e84))(&var_a4, var_a4, arg16, arg17, var_98,
        var_94, var_90, arg3, arg13, edx_3, edx_4, arg14, var_78, var_74, var_70, arg4, arg5,
        arg13, edx_5, edx_6, arg14, var_54, var_50, var_4c, arg6, arg7, arg13, edx_7, edx_8, arg14,
        var_30, var_2c, var_28, arg8, arg9, arg13, edx_9, edx_10, arg14, var_c, var_8, var_4);
    return result;
}
