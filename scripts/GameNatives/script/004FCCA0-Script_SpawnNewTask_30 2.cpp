// IGI native provenance: Script_SpawnNewTask_30 - 0x004FCCA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FCCA0-Script_SpawnNewTask_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_SpawnNewTask_30.h"
int Script_SpawnNewTask_30(void* context, float arg2, float arg3, float arg4, float arg5)

{
    float* entity_5 = arg2;
    long double fpu_val_7_1 = arg3 * entity_5[2];
    long double f1 = 0.0f;
    fpu_val_7_1 - f1;

    if (*(&((fpu_val_7_1 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
        fpu_val_7_1 = -(fpu_val_7_1);

    long double f2 = 0.0f;
    fpu_val_7_1 - f2;
    short result = (fpu_val_7_1 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f2) ? 1 : 0) << 0xa
        | (fpu_val_7_1 == f2 ? 1 : 0) << 0xe | 0x3800;

    if (*(&result + 1) & 0x40)
        return result;

    long double fpu_val_7_2 = fpu_val_7_1 * arg4;
    float temp_val = *(context + 0x24);
    float temp_1 = *(context + 0x28);
    float data_1 = *(context + 0x2c);
    float item_c = temp_1 * entity_5[2] - data_1 * entity_5[1];
    float item_8 = -((temp_val * entity_5[2] - data_1 * *entity_5));
    float item_18_1 = item_c;
    float item_14_1 = item_8;
    float item_4 = temp_val * entity_5[1] - temp_1 * *entity_5;
    long double fpu_val_6_10 = item_c;
    long double fpu_val_5_6 = item_4;
    long double fpu4 = item_8;
    arg2 = sqrtl(fpu4 * fpu4 + fpu_val_5_6 * fpu_val_5_6 + fpu_val_6_10 * fpu_val_6_10);
    long double fpu_val_6_12 = arg2;
    long double f3 = 9.99999975e-06.0f;
    fpu_val_6_12 - f3;
    float item_18_2;
    float item_14_2;
    float item_10_2;

    if (!(*(&((fpu_val_6_12 < f3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_12, f3) ? 1 : 0) << 0xa
        | (fpu_val_6_12 == f3 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
    {
        long double fpu_val_6_13 = item_c;
        long double f4_1 = 0.0f;
        fpu_val_6_13 - f4_1;
        long double fpu_val_6_18;

        if (!(*(&((fpu_val_6_13 < f4_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_13, f4_1) ? 1 : 0) << 0xa
            | (fpu_val_6_13 == f4_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
        {
        label_4fcdd0:
            long double fpu_val_6_17 = 1.0 / arg2;
            item_18_1 = item_c * fpu_val_6_17;
            item_14_1 = item_14_1 * fpu_val_6_17;
            fpu_val_6_18 = fpu_val_6_17 * item_4;
        }
        else
        {
            long double fpu_val_6_14 = item_8;
            long double f5_1 = 0.0f;
            fpu_val_6_14 - f5_1;

            if (!(*(&((fpu_val_6_14 < f5_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_14, f5_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_14 == f5_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                goto label_4fcdd0;

            long double fpu_val_6_15 = item_4;
            long double f7_1 = 0.0f;
            fpu_val_6_15 - f7_1;

            if (!(*(&((fpu_val_6_15 < f7_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_15, f7_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_15 == f7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                goto label_4fcdd0;

            fpu_val_6_18 = item_4;
        }

        item_18_2 = item_14_1 * entity_5[2] - fpu_val_6_18 * entity_5[1];
        item_14_2 = -((item_18_1 * entity_5[2] - fpu_val_6_18 * *entity_5));
        item_10_2 = item_18_1 * entity_5[1] - item_14_1 * *entity_5;
    }
    else
    {
        item_18_2 = 0.0f;
        item_14_2 = 0.0f;
        item_10_2 = 0.0f;
    }

    long double fpu_val_6_28 =
        item_18_2 * *(context + 0x18) + item_10_2 * *(context + 0x20) + item_14_2 * *(context + 0x1c);
    long double f6 = 0.0f;
    fpu_val_6_28 - f6;
    arg2 = fpu_val_6_28;
    bool c1 = /* bool c1 = unknown  {fst dword [stack_ptr+0x24], fp0} */;

    if (*(&((fpu_val_6_28 < f6 ? 1 : 0) << 8 | (c1 ? 1 : 0) << 9
            | (FCMP_UO(fpu_val_6_28, f6) ? 1 : 0) << 0xa | (fpu_val_6_28 == f6 ? 1 : 0) << 0xe
            | 0x3000) + 1) & 1)
        fpu_val_6_28 = -(fpu_val_6_28);

    fpu_val_7_2 - fpu_val_6_28;

    if (!(*(&((fpu_val_7_2 < fpu_val_6_28 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, fpu_val_6_28) ? 1 : 0) << 0xa
        | (fpu_val_7_2 == fpu_val_6_28 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41))
    {
        long double fpu_val_7_4 = arg2;
        long double f8_1 = 0.0f;
        fpu_val_7_4 - f8_1;
        fpu_val_7_2 = arg2;

        if (*(&((fpu_val_7_4 < f8_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f8_1) ? 1 : 0) << 0xa
                | (fpu_val_7_4 == f8_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
            fpu_val_7_2 = -(fpu_val_7_2);
    }

    float item_8_2 = item_14_2 * fpu_val_7_2 + *(context + 0x1c);
    long double fpu_val_7_6 = fpu_val_7_2 * item_10_2 + *(context + 0x20);
    *(context + 0x18) = item_18_2 * fpu_val_7_2 + *(context + 0x18);
    *(context + 0x1c) = item_8_2;
    *(context + 0x20) = fpu_val_7_6;
    SoundSys_InitSoundSys_17(context);
    float entity_3 = *(context + 0x38);
    long double fpu_val_7_10 = *(context + 0x34) * arg5;
    *(context + 0x30) = *(context + 0x30) * arg5;
    *(context + 0x34) = fpu_val_7_10;
    *(context + 0x38) = entity_3 * arg5;
    return context + 0x30;
}
