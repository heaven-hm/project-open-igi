// IGI native provenance: Sys_DispatchOpcode_15 - 0x004B5860 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5860-Sys_DispatchOpcode_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_DispatchOpcode_15.h"
float Sys_DispatchOpcode_15(float* context, float arg2, float arg3, float arg4, int arg5, float arg6, float arg7, float arg8, int arg9, float arg10, float arg11, float arg12, int arg13, int arg14) {
    long double fp0;
    bool c1;
    bool c2;
    fp0 = __fcos(arg10);
    float result = arg2;
    float item_8 = fp0;
    long double fp0_1;
    bool c1_1;
    bool c2_1;
    fp0_1 = __fsin(arg10);
    float item_c = fp0_1;

    if (result - 1 > 0)
    {
        float source_1 = arg8;
        float* data_val = &context[2];
        float dest_1 = arg7;
        context = data_val;
        float result_1 = result - 1;

        while (true)
        {
            long double fpu_val_7_5 = arg11 * data_val[-2];
            long double fpu_val_6_2 = arg12 * data_val[-1];
            arg2 = arg11 * *data_val;
            long double fpu_val_5_4 = arg12 * data_val[1];
            arg8 = fpu_val_5_4;
            long double fpu_val_4_1 = arg10;
            long double f1_1 = 0.0;
            fpu_val_4_1 - f1_1;

            if (!(*(&((fpu_val_4_1 < f1_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_4_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_4_1 == f1_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 0x40))
            {
                arg7 = fpu_val_7_5 * item_c + fpu_val_6_2 * item_8;
                fpu_val_7_5 = -((fpu_val_6_2 * item_c - fpu_val_7_5 * item_8));
                fpu_val_6_2 = arg7;
                fpu_val_5_4 = arg2 * item_c + arg8 * item_8;
                arg2 = -((arg8 * item_c - arg2 * item_8));
            }

            int item_48_1 = arg13;
            int item_4c_1 = arg13;
            int item_54_1 = arg13;
            int item_58_1 = arg13;
            Sys_ExecuteBytecode_15(fpu_val_7_5 + arg3, fpu_val_6_2 + arg4, arg5, arg2 + arg3, fpu_val_5_4 + arg4, arg5,
                arg6, dest_1, source_1, arg6, dest_1, source_1, arg9, arg13, arg14);
            result = result_1 - 1;
            bool flag_1_1 = result_1 != 1;
            context = &context[2];
            result_1 = result;

            if (!flag_1_1)
                break;

            data_val = context;
        }
    }

    return result;
}
