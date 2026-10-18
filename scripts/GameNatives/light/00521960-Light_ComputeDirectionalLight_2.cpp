// IGI native provenance: Light_ComputeDirectionalLight_2 - 0x00521960 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00521960-Light_ComputeDirectionalLight_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_ComputeDirectionalLight_2.hpp"
float* Light_ComputeDirectionalLight_2(int context, int arg2, int arg3, int arg4, void* arg5, float* arg6, int arg7, float* arg8, int arg9, float* arg10, float arg11, float arg12, float arg13, float arg14, float* arg15, float* arg16, float* arg17, int arg18, float* arg19, int arg20, int arg21)

{
    long double fpu7 = arg15[2];
    long double f2 = 0.0f;
    fpu7 - f2;

    if (!(*(&((fpu7 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f2) ? 1 : 0) << 0xa
        | (fpu7 == f2 ? 1 : 0) << 0xe) + 1) & 1))
    {
        long double fpu_val_7_1 = arg16[2];
        long double f3_1 = 0.0f;
        fpu_val_7_1 - f3_1;

        if (!(*(&((fpu_val_7_1 < f3_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f3_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f3_1 ? 1 : 0) << 0xe) + 1) & 1))
        {
            long double fpu_val_7_2 = arg17[2];
            long double f5_1 = 0.0f;
            fpu_val_7_2 - f5_1;

            if (!(*(&((fpu_val_7_2 < f5_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_2, f5_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_2 == f5_1 ? 1 : 0) << 0xe) + 1) & 1))
                return Light_SampleLightmap(context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
                    arg15, arg16, arg17, arg18, arg19, arg20, arg21);
        }
    }

    float item_38 = *arg15;
    int entity_5 = arg15[2];
    int item_34 = arg15[1];
    float data_11 = *arg16;
    int item_30 = entity_5;
    int entity_6 = arg16[1];
    float item_2c = data_11;
    int item_28 = entity_6;
    int item_24 = arg16[2];
    int data_13 = arg17[1];
    float item_20 = *arg17;
    int item_1c = data_13;
    int item_18 = arg17[2];
    int i_6 = 3;
    int temp_4 = *arg18;
    int item_58[0x2];
    item_58[1] = *(arg18 + 4);
    item_58[0] = temp_4;
    int temp_5 = arg19[1];
    float item_50 = *arg19;
    int item_4c = temp_5;
    int temp_6 = *(arg20 + 4);
    int item_48 = *arg20;
    int item_44 = temp_6;
    int item_64;
    int* entity_11 = &item_64;
    int* temp_7 = &item_30;
    int i;

    do
    {
        long double fpu_val_7_3 = *temp_7;
        long double f4_1 = 0.0f;
        fpu_val_7_3 - f4_1;
        int data_17;

        data_17 = !(*(&((fpu_val_7_3 < f4_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_3, f4_1) ? 1 : 0) << 0xa | (fpu_val_7_3 == f4_1 ? 1 : 0) << 0xe) + 1)
            & 0x41) ? 0 : 1;

        *entity_11 = data_17;
        temp_7 = &temp_7[3];
        entity_11 = &entity_11[1];
        i = i_6;
        i_6 -= 1;
    } while (i != 1);
    void* item_60;
    int item_5c;
    float* result = item_60 + item_5c + item_64;

    if (result == 3)
        return result;

    int saved_target;
    int item_68;

    if (result == 2)
    {
        int source_1 = 0;
        int* data_21 = &item_64;

        while (*data_21)
        {
            source_1 += 1;
            data_21 = &data_21[1];

            if (source_1 >= 3)
            {
                source_1 = item_68;
                break;
            }
        }

        int i_1 = 0;
        int (* temp_9)[0x2] = &item_58;
        int* data_22 = &item_30;

        do
        {
            if (i_1 != source_1)
            {
                long double fpu_val_7_5 = *data_22 + 4.09600019f;
                *data_22 = fpu_val_7_5;
                long double fpu_val_7_6 = 4.09600019f - (&saved_target)[source_1 * 3 + 0x12];
                long double fpu_val_6_4 = fpu_val_7_5 - (&saved_target)[source_1 * 3 + 0x12];
                *data_22 = 0x4083126f;
                long double fpu_val_7_7 = fpu_val_7_6 / fpu_val_6_4;
                data_22[-2] = (data_22[-2] - (&saved_target)[source_1 * 3 + 0x10]) * fpu_val_7_7
                    + (&saved_target)[source_1 * 3 + 0x10];
                data_22[-1] = (data_22[-1] - (&saved_target)[source_1 * 3 + 0x11]) * fpu_val_7_7
                    + (&saved_target)[source_1 * 3 + 0x11];
                *temp_9 = (*temp_9 - (&item_58)[source_1][0]) * fpu_val_7_7 + (&item_58)[source_1][0];
                item_54;
                (*temp_9)[1] = ((*temp_9)[1] - *(&item_54 + (source_1 << 3))) * fpu_val_7_7
                    + *(&item_54 + (source_1 << 3));
            }

            i_1 += 1;
            data_22 = &data_22[3];
            temp_9 = &temp_9[1];
        } while (i_1 < 3);

        int (* entity_15)[0x2] = &item_58;
        int* data_23 = &item_34;
        int i_4 = 3;
        int i_2;

        do
        {
            long double fpu_val_7_10 = 1.0f / data_23[1];
            data_23 = &data_23[3];
            entity_15 = &entity_15[1];
            i_2 = i_4;
            i_4 -= 1;
            float item_6c_1 = fpu_val_7_10;
            data_23[-2] = fpu_val_7_10;
            data_23[-4] = item_6c_1 * data_23[-4] * arg13 + arg11;
            data_23[-3] = item_6c_1 * data_23[-3] * arg14 + arg12;
            entity_15[-1][0] = item_6c_1 * entity_15[-1][0];
            (*entity_15)[-1] = item_6c_1 * (*entity_15)[-1];
        } while (i_2 != 1);
        return Light_SampleLightmap(context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, &item_38,
            &item_2c, &item_20, &item_58, &item_50, &item_48, arg21);
    }

    int source_2 = 0;
    int* data_29 = &item_64;
    int context_1;

    while (true)
    {
        if (*data_29)
        {
            context_1 = source_2;
            break;
        }

        source_2 += 1;
        data_29 = &data_29[1];

        if (source_2 >= 3)
        {
            context_1 = item_68;
            break;
        }
    }

    int f1_1 = (source_2 + 1) % 3;
    int f1_2 = (source_2 + 2) % 3;
    float item_14 = (&saved_target)[context_1 * 3 + 0x10];
    float temp_20 = (&saved_target)[context_1 * 3 + 0x11];
    float entity_22 = (&saved_target)[context_1 * 3 + 0x12];
    float item_40 = (&saved_target)[context_1 * 2 + 8];
    float entity_23 = (&saved_target)[context_1 * 2 + 9];
    long double fpu_val_7_24 = (&saved_target)[context_1 * 3 + 0x12] + 4.09600019f;
    (&saved_target)[context_1 * 3 + 0x12] = fpu_val_7_24;
    long double fpu_val_7_25 = 4.09600019f - (&saved_target)[f1_1 * 3 + 0x12];
    long double fpu_val_6_24 = fpu_val_7_24 - (&saved_target)[f1_1 * 3 + 0x12];
    (&saved_target)[context_1 * 3 + 0x12] = 0x4083126f;
    long double fpu_val_7_26 = fpu_val_7_25 / fpu_val_6_24;
    (&saved_target)[context_1 * 3 + 0x10] =
        ((&saved_target)[context_1 * 3 + 0x10] - (&saved_target)[f1_1 * 3 + 0x10]) * fpu_val_7_26
        + (&saved_target)[f1_1 * 3 + 0x10];
    (&saved_target)[context_1 * 3 + 0x11] =
        ((&saved_target)[context_1 * 3 + 0x11] - (&saved_target)[f1_1 * 3 + 0x11]) * fpu_val_7_26
        + (&saved_target)[f1_1 * 3 + 0x11];
    (&saved_target)[context_1 * 2 + 8] =
        ((&saved_target)[context_1 * 2 + 8] - (&saved_target)[f1_1 * 2 + 8]) * fpu_val_7_26
        + (&saved_target)[f1_1 * 2 + 8];
    (&saved_target)[context_1 * 2 + 9] =
        ((&saved_target)[context_1 * 2 + 9] - (&saved_target)[f1_1 * 2 + 9]) * fpu_val_7_26
        + (&saved_target)[f1_1 * 2 + 9];
    int item_c_3 = 0x4083126f;
    long double fpu_val_7_32 = (4.09600019f - (&saved_target)[f1_2 * 3 + 0x12])
        / (entity_22 + 4.09600019f - (&saved_target)[f1_2 * 3 + 0x12]);
    int (* entity_24)[0x2] = &item_58;
    int i_5 = 4;
    item_14 = (item_14 - (&saved_target)[f1_2 * 3 + 0x10]) * fpu_val_7_32
        + (&saved_target)[f1_2 * 3 + 0x10];
    float item_10_2 = (temp_20 - (&saved_target)[f1_2 * 3 + 0x11]) * fpu_val_7_32
        + (&saved_target)[f1_2 * 3 + 0x11];
    item_40 =
        (item_40 - (&saved_target)[f1_2 * 2 + 8]) * fpu_val_7_32 + (&saved_target)[f1_2 * 2 + 8];
    int* data_39 = &item_34;
    float item_3c_2 =
        (entity_23 - (&saved_target)[f1_2 * 2 + 9]) * fpu_val_7_32 + (&saved_target)[f1_2 * 2 + 9];
    int i_3;

    do
    {
        long double fpu_val_7_35 = 1.0f / data_39[1];
        data_39 = &data_39[3];
        entity_24 = &entity_24[1];
        i_3 = i_5;
        i_5 -= 1;
        float item_6c_3 = fpu_val_7_35;
        data_39[-2] = fpu_val_7_35;
        data_39[-4] = item_6c_3 * data_39[-4] * arg13 + arg11;
        data_39[-3] = item_6c_3 * data_39[-3] * arg14 + arg12;
        entity_24[-1][0] = item_6c_3 * entity_24[-1][0];
        (*entity_24)[-1] = item_6c_3 * (*entity_24)[-1];
    } while (i_3 != 1);
    Light_SampleLightmap(context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, &item_38, &item_2c,
        &item_20, &item_58, &item_50, &item_48, arg21);
    return Light_SampleLightmap(context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
        &(&saved_target)[f1_2 * 3 + 0x10], &(&saved_target)[context_1 * 3 + 0x10], &item_14,
        &(&saved_target)[f1_2 * 2 + 8], &(&saved_target)[context_1 * 2 + 8], &item_40, arg21);
}
