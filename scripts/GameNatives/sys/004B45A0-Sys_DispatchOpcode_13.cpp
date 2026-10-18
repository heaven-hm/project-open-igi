// IGI native provenance: Sys_DispatchOpcode_13 - 0x004B45A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B45A0-Sys_DispatchOpcode_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B45A0-Sys_DispatchOpcode_13.hpp"
float* Sys_DispatchOpcode_13(float* context, float arg2, float arg3, float arg4)

{
    long double fpu_val_7_1 = arg2 * 0.5f;
    long double fp0;
    bool c1;
    bool c2;
    fp0 = __fsin(fpu_val_7_1);
    context[4] = 0;
    float item_8 = fp0;
    long double fp0_1;
    bool c1_1;
    bool c2_1;
    fp0_1 = __fcos(fpu_val_7_1);
    long double fpu_val_6_3 = arg3 * 0.5f;
    long double fp0_2;
    bool c1_2;
    bool c2_2;
    fp0_2 = __fsin(fpu_val_6_3);
    float item_c = fp0_2;
    long double fp0_3;
    bool c1_3;
    bool c2_3;
    fp0_3 = __fcos(fpu_val_6_3);
    long double fpu_val_5_3 = arg4 * 0.5f;
    long double fp0_4;
    bool c1_4;
    bool c2_4;
    fp0_4 = __fsin(fpu_val_5_3);
    arg3 = fp0_4;
    long double fp0_5;
    bool c1_5;
    bool c2_5;
    fp0_5 = __fcos(fpu_val_5_3);
    arg2 = fp0_5;
    float item_4 = fp0_3 * item_8;
    arg4 = item_c * fp0_1;
    long double fpu_val_7_3 = fp0_1 * fp0_3;
    long double fpu_val_6_6 = item_c * item_8;
    *context = item_4 * arg2 - arg4 * arg3;
    context[1] = arg4 * arg2 + item_4 * arg3;
    context[2] = fpu_val_7_3 * arg3 - fpu_val_6_6 * arg2;
    context[3] = fpu_val_7_3 * arg2 + fpu_val_6_6 * arg3;
    return context;
}
