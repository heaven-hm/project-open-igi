// IGI native provenance: Human_Separation - 0x00462BA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00462BA0-Human_Separation.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Human_Separation.h"
float* Human_Separation(double* context , double* arg2, float* arg3)

{
    int* data_val = AI_CombatTarget_18();
    int item_30 = 0;

    if (*data_val > 0)
    {
        int* item_34_1 = &data_val[2];
        bool flag_1_1;

        do
        {
            void* source_1 = *item_34_1;
            short data_3;
            data_3 = *(source_1 + 0xcf6);

            if (data_3)
            {
                data_3 = *(source_1 + 0xcf5);

                if (!data_3)
                {
                    double item_20 = *arg2 - *(source_1 + 0x20);
                    double item_18_1 = arg2[1] - *(source_1 + 0x28);
                    long double fpu_val_7_6 = arg2[2] - *(source_1 + 0x30);
                    long double f0_1 = 0.0;
                    fpu_val_7_6 - f0_1;

                    if (*(&((fpu_val_7_6 < f0_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_6, f0_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_6 == f0_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                        fpu_val_7_6 = -(fpu_val_7_6);

                    long double f1_1 = 7372.8000000000002;
                    fpu_val_7_6 - f1_1;
                    data_3 = (fpu_val_7_6 < f1_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_6, f1_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_6 == f1_1 ? 1 : 0) << 0xe;

                    if (*(&data_3 + 1) & 1)
                    {
                        int item_10_1 = 0;
                        int item_c_1 = 0;
                        long double fp0_1 = Sys_OpenFileStream_11(&item_20);
                        long double f2_1 = 2252.8000000000002;
                        fp0_1 - f2_1;

                        if (*(&((fp0_1 < f2_1 ? 1 : 0) << 8
                            | (FCMP_UO(fp0_1, f2_1) ? 1 : 0) << 0xa
                            | (fp0_1 == f2_1 ? 1 : 0) << 0xe) + 1) & 1)
                        {
                            int item_10_2 = 0;
                            int item_c_2 = 0;
                            item_20 = *context - *(source_1 + 0x20);
                            double item_18_2 = context[1] - *(source_1 + 0x28);
                            long double fp0_2 = Sys_OpenFileStream_11(&item_20);
                            long double f3_1 = fp0_1;
                            fp0_2 - f3_1;

                            if (!(*(&((fp0_2 < f3_1 ? 1 : 0) << 8
                                | (FCMP_UO(fp0_2, f3_1) ? 1 : 0) << 0xa
                                | (fp0_2 == f3_1 ? 1 : 0) << 0xe) + 1) & 0x41))
                            {
                                *arg2 = *context;
                                *(arg2 + 4) = *(context + 4);
                                arg2[1] = context[1];
                                *(arg2 + 0xc) = *(context + 0xc);
                            }
                        }
                    }
                }
            }

            flag_1_1 = item_30 + 1 < *data_val;
            item_30 += 1;
            item_34_1 = &item_34_1[1];
        } while (flag_1_1);
    }

    *arg3 = *arg2 - *context;
    arg3[1] = arg2[1] - context[1];
    arg3[2] = arg2[2] - context[2];
    return arg3;
}
