// IGI native provenance: SoundSys_SetMasterVolume_16 - 0x004EC2C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EC2C0-SoundSys_SetMasterVolume_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_16.h"
float SoundSys_SetMasterVolume_16(void* context, int arg2, float arg3, void* arg4, float* arg5, float arg6) {
    float* data_val = arg6;
    float item_20 = arg5[7] * data_val[2] + arg5[1] * *data_val + arg5[4] * data_val[1];
    long double fpu_val_7_10 = arg5[8] * data_val[2] + arg5[2] * *data_val;
    long double fpu_val_6_11 = arg5[5] * data_val[1];
    float item_3c = arg5[6] * data_val[2] + arg5[3] * data_val[1] + *arg5 * *data_val;
    float result_1 = fpu_val_7_10 + fpu_val_6_11;
    float result;
    long double fp0;
    fp0 = Sys_DispatchOpcode_10(&item_3c);
    arg6 = fp0 * fp0 * arg3;
    long double fpu_val_7_14 = arg6;
    long double f0 = 0.0f;
    fpu_val_7_14 - f0;
    result = (fpu_val_7_14 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_14, f0) ? 1 : 0) << 0xa
        | (fpu_val_7_14 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x41))
    {
        long double fpu_val_7_15 = item_3c;
        long double f1_1 = 0.0f;
        fpu_val_7_15 - f1_1;
        result = result_1;
        float result_2 = result;
        float item_24_1 = item_3c;
        float item_20_1 = item_20;
        result = (fpu_val_7_15 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_15, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_15 == f1_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x40))
        {
        label_4ec3a9:
            long double fpu_val_7_18 = result_1;
            long double fpu_val_6_15 = item_20;
            long double fpu_val_5_1 = item_3c;
            long double fpu_val_7_20 =
                1.0 / sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_15 * fpu_val_6_15 + fpu_val_7_18 * fpu_val_7_18);
            item_24_1 = item_3c * fpu_val_7_20;
            item_20_1 = item_20_1 * fpu_val_7_20;
            result_2 = result_2 * fpu_val_7_20;
        }
        else
        {
            long double fpu_val_7_16 = item_20;
            long double f2_1 = 0.0f;
            fpu_val_7_16 - f2_1;
            result = (fpu_val_7_16 < f2_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_16, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_16 == f2_1 ? 1 : 0) << 0xe;

            if (!(*(&result + 1) & 0x40))
                goto label_4ec3a9;

            long double fpu_val_7_17 = result_1;
            long double f3_1 = 0.0f;
            fpu_val_7_17 - f3_1;
            result = (fpu_val_7_17 < f3_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_17, f3_1) ? 1 : 0) << 0xa
                | (fpu_val_7_17 == f3_1 ? 1 : 0) << 0xe;

            if (!(*(&result + 1) & 0x40))
                goto label_4ec3a9;
        }

        int i_1 = arg2;

        if (i_1 > 0)
        {
            int* source_1 = context + 0xc;
            int i;

            do
            {
                long double fpu_val_7_22 = source_1[-3];
                long double f4_1 = 0.0f;
                fpu_val_7_22 - f4_1;
                result = (fpu_val_7_22 < f4_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_22, f4_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_22 == f4_1 ? 1 : 0) << 0xe;

                if (!(*(&result + 1) & 0x41))
                {
                    int item_c_1 = source_1[1];
                    int item_8_1 = source_1[2];
                    int item_4_1 = source_1[3];
                    float temp_4 = *source_1;
                    long double fpu_val_7_28 = (item_20_1 * source_1[-1] + result_2 * *source_1
                        + item_24_1 * source_1[-2]) * source_1[-3] * arg6;
                    long double fpu_val_6_30 = source_1[-1] * fpu_val_7_28;
                    float item_18 = source_1[-2] * fpu_val_7_28;
                    float item_14_1 = fpu_val_6_30;
                    float item_10_1 = temp_4 * fpu_val_7_28;
                    result = ForceAccumulator_Add(arg4, &item_18);
                }

                source_1 = &source_1[7];
                i = i_1;
                i_1 -= 1;
            } while (i != 1);
        }
    }

    return result;
}
