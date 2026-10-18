// IGI native provenance: Camera_ResetMatrix_18 - 0x004A9E2C - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9E2C-Camera_ResetMatrix_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ResetMatrix_18.h"
int Camera_ResetMatrix_18(double context, int arg2, int arg3, double* arg4)

{
    int entity_val;
    int item_8 = entity_val;
    int item_c = entity_val;
    long double fpu7 = context;
    long double f0 = 0.0;
    fpu7 - f0;
    long double fpu_val_7_1 = context;
    int result = 0;

    if (TEST_BITB(
            *(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
                | (fpu7 == f0 ? 1 : 0) << 0xe | 0x3800) + 1),
            0))
        fpu_val_7_1 = -(fpu_val_7_1);

    item_c = fpu_val_7_1;

    if (arg3 == 0x7ff00000)
    {
        if (arg2)
            goto label_4a9eb7;

        long double fpu_val_7_2 = item_c;
        long double f1_1 = 1.0;
        fpu_val_7_2 - f1_1;
        short data_2 = (fpu_val_7_2 < f1_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_2, f1_1) ? 1 : 0) << 0xa | (fpu_val_7_2 == f1_1 ? 1 : 0) << 0xe;

        if (!TEST_BITB(*(&data_2 + 1), 6) && !TEST_BITB(*(&data_2 + 1), 0))
            *arg4 = g_544368;
        else
        {
            long double fpu_val_7_3 = item_c;
            long double f3_1 = 1.0;
            fpu_val_7_3 - f3_1;

            if (TEST_BITB(
                    *(&((fpu_val_7_3 < f3_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_3, f3_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_3 == f3_1 ? 1 : 0) << 0xe) + 1),
                    0))
                *arg4 = 0;
            else
            {
                result = 1;
                *arg4 = g_544370;
            }
        }
    }
    else if (arg3 != 0xfff00000 || arg2)
    {
    label_4a9eb7:

        if (*(&context + 4) != 0x7ff00000)
        {
            if (*(&context + 4) == 0xfff00000 && !context)
            {
                int item_14_1 = 0xfff00000;
                int item_18_1 = 0xfff00000;
                item_18_1 = arg2;
                int data_6 = Camera_UpdateTransform_19(0x7ff00000, 0, 0xfff00000, item_18_1);
                long double fpu_val_7_10 = arg2;
                long double f6_1 = 0.0;
                fpu_val_7_10 - f6_1;
                short data_7 = (fpu_val_7_10 < f6_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_10, f6_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_10 == f6_1 ? 1 : 0) << 0xe;

                if (TEST_BITB(*(&data_7 + 1), 6) || TEST_BITB(*(&data_7 + 1), 0))
                {
                    long double fpu_val_7_13 = arg2;
                    long double f8_1 = 0.0;
                    fpu_val_7_13 - f8_1;

                    if (!TEST_BITB(
                            *(&((fpu_val_7_13 < f8_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_13, f8_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_13 == f8_1 ? 1 : 0) << 0xe) + 1),
                            0))
                        *arg4 = 1;
                    else
                    {
                        long double fpu_val_7_14;

                        if (data_6 != 1)
                            fpu_val_7_14 = 0;
                        else
                            fpu_val_7_14 = g_544388;

                        arg2 = fpu_val_7_14;
                        *arg4 = arg2;
                    }
                }
                else
                {
                    long double fpu_val_7_11 = g_544368;

                    if (data_6 == 1)
                        fpu_val_7_11 = -(fpu_val_7_11);

                    arg2 = fpu_val_7_11;
                    *arg4 = arg2;
                }
            }
        }
        else if (!context)
        {
            long double fpu_val_7_7 = arg2;
            long double f4_1 = 0.0;
            fpu_val_7_7 - f4_1;
            short data_5 = (fpu_val_7_7 < f4_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_7, f4_1) ? 1 : 0) << 0xa
                | (fpu_val_7_7 == f4_1 ? 1 : 0) << 0xe;

            if (TEST_BITB(*(&data_5 + 1), 6) || TEST_BITB(*(&data_5 + 1), 0))
            {
                long double fpu_val_7_8 = arg2;
                long double f7_1 = 0.0;
                fpu_val_7_8 - f7_1;

                if (!TEST_BITB(
                        *(&((fpu_val_7_8 < f7_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_8, f7_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_8 == f7_1 ? 1 : 0) << 0xe) + 1),
                        0))
                    *arg4 = 1;
                else
                    *arg4 = 0;
            }
            else
                *arg4 = g_544368;
        }
    }
    else
    {
        long double fpu_val_7_5 = item_c;
        long double f2_1 = 1.0;
        fpu_val_7_5 - f2_1;
        short data_4 = (fpu_val_7_5 < f2_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_5, f2_1) ? 1 : 0) << 0xa | (fpu_val_7_5 == f2_1 ? 1 : 0) << 0xe;

        if (!TEST_BITB(*(&data_4 + 1), 6) && !TEST_BITB(*(&data_4 + 1), 0))
            *arg4 = 0;
        else
        {
            long double fpu_val_7_6 = item_c;
            long double f5_1 = 1.0;
            fpu_val_7_6 - f5_1;

            if (!TEST_BITB(
                *(&((fpu_val_7_6 < f5_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f5_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_6 == f5_1 ? 1 : 0) << 0xe) + 1),
                0))
            {
                result = 1;
                *arg4 = g_544370;
            }
            else
                *arg4 = g_544368;
        }
    }

    return result;
}
