// IGI native provenance: Sys_CheckHeap_10 - 0x004B2D90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2D90-Sys_CheckHeap_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2D90-Sys_CheckHeap_10.hpp"
float* Sys_CheckHeap_10(float* context, float* arg2, float* arg3, float* arg4, float* arg5, int* arg6)

{
    char context_val = 1;
    float item_c = *arg6;
    long double fpu7 = item_c;
    long double f1 = 0.0f;
    fpu7 - f1;
    float item_8 = arg6[1];

    if (1 & *(&((fpu7 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f1) ? 1 : 0) << 0xa
            | (fpu7 == f1 ? 1 : 0) << 0xe) + 1))
        item_c = -(item_c);

    long double fpu_val_7_3 = item_8;
    long double f2 = 0.0f;
    fpu_val_7_3 - f2;

    if (*(&((fpu_val_7_3 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f2) ? 1 : 0) << 0xa
            | (fpu_val_7_3 == f2 ? 1 : 0) << 0xe) + 1) & 1)
        item_8 = -(item_8);

    long double fpu_val_7_6 = arg6[2];
    long double f3 = 0.0f;
    fpu_val_7_6 - f3;

    if (*(&((fpu_val_7_6 < f3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f3) ? 1 : 0) << 0xa
            | (fpu_val_7_6 == f3 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
        fpu_val_7_6 = -(fpu_val_7_6);

    long double fpu6 = item_8;
    long double f4 = item_c;
    fpu6 - f4;
    long double fpu_val_6_1;

    if (!(*(&((fpu6 < f4 ? 1 : 0) << 8 | (FCMP_UO(fpu6, f4) ? 1 : 0) << 0xa
        | (fpu6 == f4 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
    {
        fpu_val_6_1 = item_8;
        fpu_val_6_1 - fpu_val_7_6;
    }

    long double fpu_val_6_14;
    long double fpu_val_7_11;

    if (!(*(&((fpu6 < f4 ? 1 : 0) << 8 | (FCMP_UO(fpu6, f4) ? 1 : 0) << 0xa
        | (fpu6 == f4 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41) && !(*(&(
        (fpu_val_6_1 < fpu_val_7_6 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fpu_val_7_6) ? 1 : 0) << 0xa
        | (fpu_val_6_1 == fpu_val_7_6 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
    {
        long double fpu_val_7_10 = arg5[2] * *arg4 - *arg5 * arg4[2];
        long double f8_1 = 0.0f;
        fpu_val_7_10 - f8_1;

        if (*(&((fpu_val_7_10 < f8_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_10, f8_1) ? 1 : 0) << 0xa
                | (fpu_val_7_10 == f8_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
            context_val = 0;

        long double fpu_val_6_6 = *arg4 * arg3[2] - *arg3 * arg4[2];
        fpu_val_7_11 = fpu_val_6_6 / fpu_val_6_6;
        long double f11_1 = -9.99999975e-06.0f;
        fpu_val_7_11 - f11_1;
        long double f14_1;

        if (!(*(&((fpu_val_7_11 < f11_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_11, f11_1) ? 1 : 0) << 0xa
            | (fpu_val_7_11 == f11_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
        {
            f14_1 = 1.00001001f;
            fpu_val_7_11 - f14_1;
        }

        if (*(&((fpu_val_7_11 < f11_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f11_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f11_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1 || !(*(&(
                (fpu_val_7_11 < f14_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f14_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f14_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
            context_val = 0;

        long double fpu_val_6_7 = *arg4;
        long double f17_1 = 0.0f;
        fpu_val_6_7 - f17_1;
        long double fpu_val_6_8 = *arg4;

        if (*(&((fpu_val_6_7 < f17_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_7, f17_1) ? 1 : 0) << 0xa
                | (fpu_val_6_7 == f17_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
            fpu_val_6_8 = -(fpu_val_6_8);

        long double fpu_val_5_2 = arg4[2];
        long double f20_1 = 0.0f;
        fpu_val_5_2 - f20_1;
        long double fpu_val_5_3 = arg4[2];

        if (*(&((fpu_val_5_2 < f20_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_5_2, f20_1) ? 1 : 0) << 0xa
                | (fpu_val_5_2 == f20_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
            fpu_val_5_3 = -(fpu_val_5_3);

        fpu_val_6_8 - fpu_val_5_3;

        if (*(&((fpu_val_6_8 < fpu_val_5_3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_8, fpu_val_5_3) ? 1 : 0) << 0xa
            | (fpu_val_6_8 == fpu_val_5_3 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41)
        {
            long double fpu_val_6_15 = arg4[2];
            long double f23_1 = 0.0f;
            fpu_val_6_15 - f23_1;

            if (*(&((fpu_val_6_15 < f23_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_15, f23_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_15 == f23_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
                context_val = 0;

            fpu_val_6_14 = (arg3[2] - fpu_val_7_11 * arg5[2]) / arg4[2];
        }
        else
            fpu_val_6_14 = (*arg3 - fpu_val_7_11 * *arg5) / *arg4;

        goto label_4b2f8e;
    }

    long double f5_1 = item_c;
    fpu_val_7_6 - f5_1;

    if (*(&((fpu_val_7_6 < f5_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f5_1) ? 1 : 0) << 0xa
        | (fpu_val_7_6 == f5_1 ? 1 : 0) << 0xe) + 1) & 0x41)
    {
        long double fpu_val_7_17 = arg5[2] * arg4[1] - arg4[2] * arg5[1];
        long double f6_1 = 0.0f;
        fpu_val_7_17 - f6_1;

        if (*(&((fpu_val_7_17 < f6_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_17, f6_1) ? 1 : 0) << 0xa
                | (fpu_val_7_17 == f6_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
            context_val = 0;

        long double fpu_val_6_39 = arg3[2] * arg4[1] - arg3[1] * arg4[2];
        fpu_val_7_11 = fpu_val_6_39 / fpu_val_6_39;
        long double f9_1 = -9.99999975e-06.0f;
        fpu_val_7_11 - f9_1;
        long double f12_1;

        if (!(*(&((fpu_val_7_11 < f9_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f9_1) ? 1 : 0) << 0xa
            | (fpu_val_7_11 == f9_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
        {
            f12_1 = 1.00001001f;
            fpu_val_7_11 - f12_1;
        }

        if (*(&((fpu_val_7_11 < f9_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f9_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f9_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1 || !(*(&(
                (fpu_val_7_11 < f12_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f12_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f12_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
            context_val = 0;

        long double fpu_val_6_40 = arg4[1];
        long double f15_1 = 0.0f;
        fpu_val_6_40 - f15_1;
        long double fpu_val_6_41 = arg4[1];

        if (*(&((fpu_val_6_40 < f15_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_40, f15_1) ? 1 : 0) << 0xa
                | (fpu_val_6_40 == f15_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
            fpu_val_6_41 = -(fpu_val_6_41);

        long double fpu_val_5_14 = arg4[2];
        long double f18_1 = 0.0f;
        fpu_val_5_14 - f18_1;
        long double fpu_val_5_15 = arg4[2];

        if (*(&((fpu_val_5_14 < f18_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_5_14, f18_1) ? 1 : 0) << 0xa
                | (fpu_val_5_14 == f18_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
            fpu_val_5_15 = -(fpu_val_5_15);

        fpu_val_6_41 - fpu_val_5_15;

        if (*(&((fpu_val_6_41 < fpu_val_5_15 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_6_41, fpu_val_5_15) ? 1 : 0) << 0xa
            | (fpu_val_6_41 == fpu_val_5_15 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41)
        {
            long double fpu_val_6_47 = arg4[2];
            long double f21_1 = 0.0f;
            fpu_val_6_47 - f21_1;

            if (*(&((fpu_val_6_47 < f21_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_47, f21_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_47 == f21_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
                context_val = 0;

            fpu_val_6_14 = (arg3[2] - fpu_val_7_11 * arg5[2]) / arg4[2];
        }
        else
            fpu_val_6_14 = (arg3[1] - fpu_val_7_11 * arg5[1]) / arg4[1];

        long double f24_1 = -9.99999975e-06.0f;
        fpu_val_6_14 - f24_1;

        if (*(&((fpu_val_6_14 < f24_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_14, f24_1) ? 1 : 0) << 0xa
                | (fpu_val_6_14 == f24_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
            context_val = 0;
        else
        {
            long double fpu_val_5_18 = fpu_val_6_14 + fpu_val_7_11;
            long double f26_1 = 1.00001001f;
            fpu_val_5_18 - f26_1;

            if (!(*(&((fpu_val_5_18 < f26_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_5_18, f26_1) ? 1 : 0) << 0xa
                    | (fpu_val_5_18 == f26_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41))
                context_val = 0;
        }
    }
    else
    {
        long double fpu_val_7_14 = *arg4 * arg5[1] - *arg5 * arg4[1];
        long double f7_1 = 0.0f;
        fpu_val_7_14 - f7_1;

        if (*(&((fpu_val_7_14 < f7_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_14, f7_1) ? 1 : 0) << 0xa
                | (fpu_val_7_14 == f7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
            context_val = 0;

        long double fpu_val_6_23 = *arg4 * arg3[1] - *arg3 * arg4[1];
        fpu_val_7_11 = fpu_val_6_23 / fpu_val_6_23;
        long double f10_1 = -9.99999975e-06.0f;
        fpu_val_7_11 - f10_1;
        long double f13_1;

        if (!(*(&((fpu_val_7_11 < f10_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_11, f10_1) ? 1 : 0) << 0xa
            | (fpu_val_7_11 == f10_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
        {
            f13_1 = 1.00001001f;
            fpu_val_7_11 - f13_1;
        }

        if (*(&((fpu_val_7_11 < f10_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f10_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f10_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1 || !(*(&(
                (fpu_val_7_11 < f13_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_11, f13_1) ? 1 : 0) << 0xa
                | (fpu_val_7_11 == f13_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
            context_val = 0;

        long double fpu_val_6_24 = *arg4;
        long double f16_1 = 0.0f;
        fpu_val_6_24 - f16_1;
        long double fpu_val_6_25 = *arg4;

        if (*(&((fpu_val_6_24 < f16_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_24, f16_1) ? 1 : 0) << 0xa
                | (fpu_val_6_24 == f16_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
            fpu_val_6_25 = -(fpu_val_6_25);

        long double fpu_val_5_7 = arg4[1];
        long double f19_1 = 0.0f;
        fpu_val_5_7 - f19_1;
        long double fpu_val_5_8 = arg4[1];

        if (*(&((fpu_val_5_7 < f19_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_5_7, f19_1) ? 1 : 0) << 0xa
                | (fpu_val_5_7 == f19_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
            fpu_val_5_8 = -(fpu_val_5_8);

        fpu_val_6_25 - fpu_val_5_8;

        if (*(&((fpu_val_6_25 < fpu_val_5_8 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_25, fpu_val_5_8) ? 1 : 0) << 0xa
            | (fpu_val_6_25 == fpu_val_5_8 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41)
        {
            long double fpu_val_6_31 = arg4[1];
            long double f22_1 = 0.0f;
            fpu_val_6_31 - f22_1;

            if (*(&((fpu_val_6_31 < f22_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_31, f22_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_31 == f22_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
                context_val = 0;

            fpu_val_6_14 = (arg3[1] - fpu_val_7_11 * arg5[1]) / arg4[1];
        }
        else
            fpu_val_6_14 = (*arg3 - fpu_val_7_11 * *arg5) / *arg4;

    label_4b2f8e:
        long double f25_1 = -9.99999975e-06.0f;
        fpu_val_6_14 - f25_1;

        if (*(&((fpu_val_6_14 < f25_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_14, f25_1) ? 1 : 0) << 0xa
                | (fpu_val_6_14 == f25_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
            context_val = 0;
        else
        {
            long double fpu_val_5_11 = fpu_val_6_14 + fpu_val_7_11;
            long double f27_1 = 1.00001001f;
            fpu_val_5_11 - f27_1;

            if (!(*(&((fpu_val_5_11 < f27_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_5_11, f27_1) ? 1 : 0) << 0xa
                    | (fpu_val_5_11 == f27_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41))
                context_val = 0;
        }
    }

    if (context)
        *context = fpu_val_6_14;

    float* result = arg2;

    if (!result)
    {
        result = context_val;
        return result;
    }

    *result = fpu_val_7_11;
    result = context_val;
    return result;
}
