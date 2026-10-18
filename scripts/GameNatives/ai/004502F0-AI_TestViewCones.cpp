// IGI native provenance: AI_TestViewCones - 0x004502F0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004502F0-AI_TestViewCones.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004502F0-AI_TestViewCones.hpp"
double* AI_TestViewCones(long double context , long double arg2 , long double arg3 , void* arg4, double* arg5, char arg6)

{
    double* result = arg5;
    long double fpu_val_7_1 = *result - *(arg4 + 0x18);
    double item_38 = fpu_val_7_1;
    long double fpu_val_6_1 = result[1] - *(arg4 + 0x20);
    double item_30 = fpu_val_6_1;
    long double fpu_val_5_1 = result[2] - *(arg4 + 0x28);
    double item_28 = fpu_val_5_1;
    double item_20 =
        *(arg4 + 0x70) * fpu_val_5_1 + *(arg4 + 0x64) * fpu_val_6_1 + *(arg4 + 0x58) * fpu_val_7_1;
    double item_18 =
        *(arg4 + 0x74) * fpu_val_5_1 + *(arg4 + 0x68) * fpu_val_6_1 + *(arg4 + 0x5c) * fpu_val_7_1;
    long double fpu_val_3_11 = *(arg4 + 0x60) * fpu_val_7_1;
    double item_10 = *(arg4 + 0x78) * fpu_val_5_1 + *(arg4 + 0x6c) * fpu_val_6_1 + fpu_val_3_11;
    memcpy(&item_38, &item_20, 0x18);
    long double fpu_val_7_3 = item_18;
    long double f0 = 0.0;
    fpu_val_7_3 - f0;
    result = (fpu_val_7_3 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 1))
    {
        double item_40_1 = Sys_OpenFileStream_11(&item_38);
        int item_48_1;

        if (!arg6)
        {
            item_48_1 = *(arg4 + 0x90);
            int item_44_2 = *(arg4 + 0x94);
        }
        else
        {
            item_48_1 = *(arg4 + 0x98);
            int item_44_1 = *(arg4 + 0x9c);
        }

        void item_70;
        memcpy(&item_70, &item_38, 0x18);
        WeaponType_InitType();
        long double f1_1 = 0.0f;
        context - f1_1;

        if (!(*(&((context < f1_1 ? 1 : 0) << 8 | (FCMP_UO(context, f1_1) ? 1 : 0) << 0xa
            | (context == f1_1 ? 1 : 0) << 0xe | 0x800) + 1) & 1))
        {
            memcpy(&item_70, &item_38, 0x18);
            WeaponType_InitType();
        }
        else
        {
            memcpy(&item_70, &item_38, 0x18);
            WeaponType_InitType();
            arg2 = -(arg2);
        }

        float item_4c_1 = arg2;
        memcpy(&item_70, &item_38, 0x18);
        WeaponType_GetConfig();
        long double f2_1 = 0.0f;
        arg3 - f2_1;

        if (!(*(&((arg3 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(arg3, f2_1) ? 1 : 0) << 0xa
            | (arg3 == f2_1 ? 1 : 0) << 0xe | 0x1800) + 1) & 1))
        {
            memcpy(&item_70, &item_38, 0x18);
            result = WeaponType_GetConfig();
        }
        else
        {
            memcpy(&item_70, &item_38, 0x18);
            result = WeaponType_GetConfig();
            fpu_val_3_11 = -(fpu_val_3_11);
        }

        long double fpu2 = item_40_1;
        long double f3_1 = item_48_1;
        fpu2 - f3_1;
        result = (fpu2 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(fpu2, f3_1) ? 1 : 0) << 0xa
            | (fpu2 == f3_1 ? 1 : 0) << 0xe | 0x1800;

        if (*(&result + 1) & 0x41)
        {
            long double fpu_val_2_1 = item_4c_1;
            long double f4_1 = *(arg4 + 0x80);
            fpu_val_2_1 - f4_1;
            result = (fpu_val_2_1 < f4_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_2_1, f4_1) ? 1 : 0) << 0xa
                | (fpu_val_2_1 == f4_1 ? 1 : 0) << 0xe | 0x1800;

            if (*(&result + 1) & 1)
            {
                long double f5_1 = *(arg4 + 0x88);
                fpu_val_3_11 - f5_1;
                result = (fpu_val_3_11 < f5_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_3_11, f5_1) ? 1 : 0) << 0xa
                    | (fpu_val_3_11 == f5_1 ? 1 : 0) << 0xe | 0x1800;

                if (*(&result + 1) & 1)
                {
                    result = 1;
                    return result;
                }
            }
        }

        result = arg6;
        long double fpu_val_2_2;

        if (!result)
            fpu_val_2_2 = *(arg4 + 0xb0);
        else
            fpu_val_2_2 = *(arg4 + 0xb8);

        long double fpu1 = item_40_1;
        fpu1 - fpu_val_2_2;
        result = (fpu1 < fpu_val_2_2 ? 1 : 0) << 8 | (FCMP_UO(fpu1, fpu_val_2_2) ? 1 : 0) << 0xa
            | (fpu1 == fpu_val_2_2 ? 1 : 0) << 0xe | 0x1000;

        if (*(&result + 1) & 0x41)
        {
            long double fpu_val_2_4 = item_4c_1;
            long double f6_1 = *(arg4 + 0xa0);
            fpu_val_2_4 - f6_1;
            result = (fpu_val_2_4 < f6_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_2_4, f6_1) ? 1 : 0) << 0xa
                | (fpu_val_2_4 == f6_1 ? 1 : 0) << 0xe | 0x1800;

            if (*(&result + 1) & 1)
            {
                long double f7_1 = *(arg4 + 0xa8);
                fpu_val_3_11 - f7_1;
                result = (fpu_val_3_11 < f7_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_3_11, f7_1) ? 1 : 0) << 0xa
                    | (fpu_val_3_11 == f7_1 ? 1 : 0) << 0xe | 0x2000;

                if (*(&result + 1) & 1)
                {
                    result = 1;
                    return result;
                }
            }
        }
    }

    result = 0;
    return result;
}
