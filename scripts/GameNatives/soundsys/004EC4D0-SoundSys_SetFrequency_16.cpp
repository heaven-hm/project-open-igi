// IGI native provenance: SoundSys_SetFrequency_16 - 0x004EC4D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EC4D0-SoundSys_SetFrequency_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetFrequency_16.hpp"
int* SoundSys_SetFrequency_16(int* context, int arg2, int arg3, int arg4, int arg5, float arg6, float arg7, float arg8)

{
    long double fpu7 = arg6;
    long double f0 = 0.0f;
    fpu7 - f0;
    float item_c = arg6;
    float item_8 = arg7;
    float item_4 = arg8;

    if (!(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40))
    {
    label_4ec51e:
        long double fpu_val_7_3 = arg8;
        long double fpu_val_6_1 = arg7;
        long double fpu_val_5_1 = arg6;
        long double fpu_val_7_5 =
            1.0 / sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_3 * fpu_val_7_3);
        item_c = arg6 * fpu_val_7_5;
        item_8 = arg7 * fpu_val_7_5;
        item_4 = arg8 * fpu_val_7_5;
    }
    else
    {
        long double fpu_val_7_1 = arg7;
        long double f1_1 = 0.0f;
        fpu_val_7_1 - f1_1;

        if (!(*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_4ec51e;

        long double fpu_val_7_2 = arg8;
        long double f2_1 = 0.0f;
        fpu_val_7_2 - f2_1;

        if (!(*(&((fpu_val_7_2 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_2 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_4ec51e;
    }

    context[1] = item_c;
    context[2] = item_8;
    context[3] = item_4;
    context[4] = arg3;
    context[5] = arg4;
    context[6] = arg5;
    *context = arg2;
    return context;
}
