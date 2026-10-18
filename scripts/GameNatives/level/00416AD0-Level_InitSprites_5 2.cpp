// IGI native provenance: Level_InitSprites_5 - 0x00416AD0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00416AD0-Level_InitSprites_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Level_InitSprites_5.h"
int Level_InitSprites_5(long double context , long double arg2 , long double arg3 , long double arg4 , void* arg5, int arg6, float arg7)

{
    double* data_val = arg7;
    double item_30 = *data_val - *(arg5 + 8);
    double item_28 = data_val[1] - *(arg5 + 0x10);
    double item_20 = data_val[2] - *(arg5 + 0x18);
    long double fp0 = Sys_OpenFileStream_11(&item_30);
    long double f0 = *(arg5 + 0x78);
    fp0 - f0;
    short result = (fp0 < f0 ? 1 : 0) << 8 | (FCMP_UO(fp0, f0) ? 1 : 0) << 0xa
        | (fp0 == f0 ? 1 : 0) << 0xe;

    if (*(&result + 1) & 1)
    {
        double item_18 = *(arg5 + 0x38) * item_20 + *(arg5 + 0x2c) * item_28 + *(arg5 + 0x20) * item_30;
        long double fpu_val_7_14 =
            *(arg5 + 0x3c) * item_20 + *(arg5 + 0x30) * item_28 + *(arg5 + 0x24) * item_30;
        double item_10_1 = fpu_val_7_14;
        double item_8_1 =
            *(arg5 + 0x40) * item_20 + *(arg5 + 0x34) * item_28 + *(arg5 + 0x28) * item_30;
        memcpy(&item_30, &item_18, 0x18);
        long double f1_1 = 0.0;
        fpu_val_7_14 - f1_1;
        result = (fpu_val_7_14 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_14, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_14 == f1_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x41))
        {
            void item_54;
            memcpy(&item_54, &item_18, 0x18);
            WeaponType_InitType();
            long double f2_1 = 0.0f;
            context - f2_1;

            if (!(*(&((context < f2_1 ? 1 : 0) << 8 | (FCMP_UO(context, f2_1) ? 1 : 0) << 0xa
                | (context == f2_1 ? 1 : 0) << 0xe | 0x800) + 1) & 1))
            {
                memcpy(&item_54, &item_30, 0x18);
                WeaponType_InitType();
            }
            else
            {
                memcpy(&item_54, &item_30, 0x18);
                WeaponType_InitType();
                arg2 = -(arg2);
            }

            arg7 = arg2;
            memcpy(&item_54, &item_30, 0x18);
            WeaponType_GetConfig();
            long double f3_1 = 0.0f;
            arg3 - f3_1;

            if (!(*(&((arg3 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(arg3, f3_1) ? 1 : 0) << 0xa
                | (arg3 == f3_1 ? 1 : 0) << 0xe | 0x1800) + 1) & 1))
            {
                memcpy(&item_54, &item_30, 0x18);
                WeaponType_GetConfig();
            }
            else
            {
                memcpy(&item_54, &item_30, 0x18);
                WeaponType_GetConfig();
                arg4 = -(arg4);
            }

            long double fpu2 = arg7;
            long double f4_1 = *(arg5 + 0x70);
            fpu2 - f4_1;
            result = (fpu2 < f4_1 ? 1 : 0) << 8 | (FCMP_UO(fpu2, f4_1) ? 1 : 0) << 0xa
                | (fpu2 == f4_1 ? 1 : 0) << 0xe | 0x1800;

            if (*(&result + 1) & 1)
            {
                long double f5_1 = *(arg5 + 0x74);
                arg4 - f5_1;
                result = (arg4 < f5_1 ? 1 : 0) << 8 | (FCMP_UO(arg4, f5_1) ? 1 : 0) << 0xa
                    | (arg4 == f5_1 ? 1 : 0) << 0xe | 0x1800;

                if (*(&result + 1) & 1)
                {
                    *(arg5 + 0x74) = arg4;
                    *(arg5 + 0x70) = arg7;
                    *(arg5 + 0x80) = arg6;
                    result = 1;
                    return result;
                }
            }
        }
    }

    result = 0;
    return result;
}
