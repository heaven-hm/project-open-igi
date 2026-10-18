// IGI native provenance: Camera_ApplyLineFilter_2 - 0x004A2800 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A2800-Camera_ApplyLineFilter_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_2.h"
int Camera_ApplyLineFilter_2(void* context, float arg2, float arg3, float arg4)

{
    long double fpu7 = 0.0f;
    long double f0 = arg2;
    fpu7 - f0;
    long double fpu_val_7_1;

    if (*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x41)
    {
        long double fpu_val_7_2 = 1.0f;
        long double f1_1 = arg2;
        fpu_val_7_2 - f1_1;

        fpu_val_7_1 = !(*(&((fpu_val_7_2 < f1_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_2, f1_1) ? 1 : 0) << 0xa | (fpu_val_7_2 == f1_1 ? 1 : 0) << 0xe) + 1)
            & 1) ? arg2 : 1.0f;
    }
    else
        fpu_val_7_1 = 0.0f;

    *(context + 0x64) = fpu_val_7_1;
    long double fpu_val_7_3 = 0.0f;
    long double f2 = arg3;
    fpu_val_7_3 - f2;
    long double fpu_val_7_4;

    if (*(&((fpu_val_7_3 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f2) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f2 ? 1 : 0) << 0xe) + 1) & 0x41)
    {
        long double fpu_val_7_5 = 1.0f;
        long double f3_1 = arg3;
        fpu_val_7_5 - f3_1;

        fpu_val_7_4 = !(*(&((fpu_val_7_5 < f3_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_5, f3_1) ? 1 : 0) << 0xa | (fpu_val_7_5 == f3_1 ? 1 : 0) << 0xe) + 1)
            & 1) ? arg3 : 1.0f;
    }
    else
        fpu_val_7_4 = 0.0f;

    *(context + 0x68) = fpu_val_7_4;
    long double fpu_val_7_6 = 0.0f;
    long double f4 = arg4;
    fpu_val_7_6 - f4;
    short data_val = (fpu_val_7_6 < f4 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f4) ? 1 : 0) << 0xa
        | (fpu_val_7_6 == f4 ? 1 : 0) << 0xe;

    if (!(*(&data_val + 1) & 0x41))
    {
        *(context + 0x6c) = 0;
        return data_val;
    }

    long double fpu_val_7_7 = 1.0f;
    long double f5 = arg4;
    fpu_val_7_7 - f5;
    data_val = (fpu_val_7_7 < f5 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_7, f5) ? 1 : 0) << 0xa
        | (fpu_val_7_7 == f5 ? 1 : 0) << 0xe;

    if (!(*(&data_val + 1) & 1))
    {
        *(context + 0x6c) = arg4;
        return arg4;
    }

    *(context + 0x6c) = 0x3f800000;
    return data_val;
}
