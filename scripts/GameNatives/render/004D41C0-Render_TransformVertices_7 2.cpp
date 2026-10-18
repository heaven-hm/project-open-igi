// IGI native provenance: Render_TransformVertices_7 - 0x004D41C0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004D41C0-Render_TransformVertices_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_TransformVertices_7.h"
float Render_TransformVertices_7(float context, int arg2, float arg3, float arg4, float arg5, float arg6) {
    if (arg2 == 0xffffffff)
    {
        if (arg4 == 0xffffffff)
            return 0xffffffff;

        memcpy(context, arg5, 0x14);
        return arg4;
    }

    if (arg4 == 0xffffffff)
    {
        memcpy(context, arg3, 0x14);
        return arg2;
    }

    if (arg2)
    {
        if (arg4 == 1)
        {
            Render_ComputeNormals_7(context, arg3, arg5, arg6);
            return 1;
        }

        float data_10 = arg3;
        float entity_5 = arg5;
        float* temp_4 = context;
        long double fpu_val_7_10 = (*(data_10 + 8) - *(data_10 + 4)) * (*(entity_5 + 4) - *(entity_5 + 8));
        long double fpu_val_6_24 = (*(data_10 + 0xc) - *data_10) * (*(entity_5 + 4) + *(entity_5 + 8));
        arg4 = (*(entity_5 + 0xc) - *entity_5) * (*(data_10 + 8) + *(data_10 + 4));
        arg6 = (*(entity_5 + 4) + *entity_5) * (*data_10 + *(data_10 + 8));
        arg5 = (*(entity_5 + 0xc) - *(entity_5 + 8)) * (*(data_10 + 0xc) + *(data_10 + 4));
        long double fpu_val_5_44 = (*(entity_5 + 0xc) + *(entity_5 + 8)) * (*(data_10 + 0xc) - *(data_10 + 4));
        long double fpu_val_4_23 = fpu_val_5_44 + arg5 + arg6;
        arg3 = ((*entity_5 - *(entity_5 + 4)) * (*(data_10 + 8) - *data_10) + fpu_val_4_23) * 0.5f;
        *temp_4 = (*(entity_5 + 0xc) + *entity_5) * (*data_10 + *(data_10 + 0xc)) + arg3 - fpu_val_4_23;
        long double fpu_val_5_46 = fpu_val_6_24 + arg3 - fpu_val_5_44;
        context = fpu_val_5_46;
        temp_4[1] = fpu_val_5_46;
        long double fpu_val_6_30 = arg3 + arg4 - arg5;
        arg5 = fpu_val_6_30;
        temp_4[2] = fpu_val_6_30;
        long double fpu_val_7_12 = fpu_val_7_10 + arg3 - arg6;
        temp_4[3] = fpu_val_7_12;
        int data_12 = *(data_10 + 0x10) + *(entity_5 + 0x10) + 1;
        temp_4[4] = data_12;

        if (data_12 <= 0x3e8)
            return 1;

        long double fpu_val_6_31 = *temp_4;
        temp_4[4] = 0;
        long double fpu_val_5_55 =
            1.0 / sqrtl(fpu_val_7_12 * fpu_val_7_12 + arg5 * arg5 + context * context + fpu_val_6_31 * fpu_val_6_31);
        *temp_4 = *temp_4 * fpu_val_5_55;
        temp_4[1] = context * fpu_val_5_55;
        temp_4[2] = arg5 * fpu_val_5_55;
        temp_4[3] = fpu_val_7_12 * fpu_val_5_55;
        return 1;
    }

    if (!arg4)
    {
        Render_ComputeNormals_7(context, arg3, arg5, arg6);
        return 0;
    }

    float* data_3 = arg3;
    float* entity_2 = arg5;
    float* temp_1 = context;
    long double fpu_val_7_2 = (data_3[1] - data_3[2]) * (entity_2[2] - entity_2[1]);
    long double fpu_val_6_4 = (entity_2[3] - *entity_2) * (data_3[2] + data_3[1]);
    arg4 = (data_3[3] - *data_3) * (entity_2[1] + entity_2[2]);
    arg6 = (*data_3 + data_3[1]) * (*entity_2 + entity_2[2]);
    arg5 = (data_3[3] - data_3[2]) * (entity_2[3] + entity_2[1]);
    long double fpu_val_5_13 = (data_3[3] + data_3[2]) * (entity_2[3] - entity_2[1]);
    long double fpu_val_4_10 = fpu_val_5_13 + arg5 + arg6;
    arg3 = ((*data_3 - data_3[1]) * (entity_2[2] - *entity_2) + fpu_val_4_10) * 0.5f;
    *temp_1 = (entity_2[3] + *entity_2) * (*data_3 + data_3[3]) + arg3 - fpu_val_4_10;
    long double fpu_val_5_15 = fpu_val_6_4 + arg3 - fpu_val_5_13;
    context = fpu_val_5_15;
    temp_1[1] = fpu_val_5_15;
    long double fpu_val_6_10 = arg3 + arg4 - arg5;
    arg5 = fpu_val_6_10;
    temp_1[2] = fpu_val_6_10;
    long double fpu_val_7_4 = fpu_val_7_2 + arg3 - arg6;
    temp_1[3] = fpu_val_7_4;
    int data_5 = data_3[4] + entity_2[4] + 1;
    temp_1[4] = data_5;

    if (data_5 <= 0x3e8)
        return 1;

    long double fpu_val_6_11 = *temp_1;
    temp_1[4] = 0;
    long double fpu_val_6_17 =
        1.0 / sqrtl(fpu_val_6_11 * fpu_val_6_11 + fpu_val_7_4 * fpu_val_7_4 + arg5 * arg5 + context * context);
    *temp_1 = *temp_1 * fpu_val_6_17;
    temp_1[1] = context * fpu_val_6_17;
    temp_1[2] = arg5 * fpu_val_6_17;
    temp_1[3] = fpu_val_7_4 * fpu_val_6_17;
    return 1;
}
