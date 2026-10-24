// IGI native provenance: Render_SkinBones_8 - 0x004D4B60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D4B60-Render_SkinBones_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D4B60-Render_SkinBones_8.hpp"
float Render_SkinBones_8(int context, float arg2, float arg3, float arg4) {
    float source_val = arg4;
    void* data_val = *(source_val + 0x30);

    if (data_val)
        *(data_val + 4) = *(source_val + 0x2c);

    int* dest_val = arg3;
    *(source_val + 8) &= 0xfffffeff;
    dest_val[1] &= 0xfffffeff;
    float temp_2 = *(source_val + 0x2c);
    arg3 = *(source_val + 0x28) + *(source_val + 0x2c);
    *(source_val + 0x2c) = arg3;
    float data_1;

    if (context)
        data_1 = *(*(source_val + 0xc) + 0x10);

    int entity_5;
    float result;

    if (!context || !data_1 || *data_1 <= 0)
    {
        result = arg2;
        entity_5 = 0;
    }
    else
    {
        long double fpu_val_7_2;
        long double f2_1;

        if (*((source_val + 8) + 1) & 0x80)
        {
            fpu_val_7_2 = temp_2;
            f2_1 = arg3;
            fpu_val_7_2 - f2_1;
        }

        float data_4;

        if (!(*((source_val + 8) + 1) & 0x80) || *(&((fpu_val_7_2 < f2_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_2, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_2 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40)
            data_4 = arg2;
        else
            data_4 = Render_ComputeNormals_8(context, arg2, dest_val, source_val, temp_2, temp_2);

        entity_5 = 1;
        result = Render_ComputeNormals_8(context, data_4, dest_val, source_val, temp_2, *(source_val + 0x2c));
    }

    long double fpu_val_7_3 = *(source_val + 0x2c);
    long double f0 = **(source_val + 0xc);
    fpu_val_7_3 - f0;
    int data_6;

    if (!(*(&((fpu_val_7_3 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0) ? 1 : 0) << 0xa
            | (fpu_val_7_3 == f0 ? 1 : 0) << 0xe) + 1) & 1))
        data_6 = *(source_val + 8);

    if (!(*(&((fpu_val_7_3 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f0) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f0 ? 1 : 0) << 0xe) + 1) & 1) && !(data_6 & 2))
    {
    label_4d4c4a:
        *(&data_6 + 1) |= 1;
        *(source_val + 8) = data_6;
        int data_7 = dest_val[1];
        *(&data_7 + 1) |= 1;
        dest_val[1] = data_7;
        char data_8 = *(source_val + 8);

        if (!(data_8 & 1))
        {
            if (data_8 & 2)
            {
                int data_23 = *(source_val + 8);
                *(source_val + 0x2c) = 0;
                *(&data_23 + 1) &= 0x7f;
                *(source_val + 8) = data_23;
                return result;
            }

            *(source_val + 0x2c) = **(source_val + 0xc);
        }
        else
        {
            float* data_9 = *(source_val + 0xc);

            if (!(data_8 & 2))
            {
                arg4 = -1.0f;
                *(source_val + 0x2c) = *(source_val + 0x2c) - *data_9;
            }
            else
            {
                *(source_val + 0x2c) = *data_9 + *(source_val + 0x2c);
                arg4 = *data_9 + 1.0f;
            }

            if (entity_5)
                result = Render_ComputeNormals_8(context, result, dest_val, source_val, arg4, *(source_val + 0x2c));

            if (!*dest_val)
            {
                short entity_8 = *(source_val + 8);

                if (!(*(&entity_8 + 1) & 0x80))
                {
                    int* data_11 = *(source_val + 0xc);
                    float temp_6 = *(source_val + 0x2c);
                    int* dest_1 = data_11[2];

                    if (!(entity_8 & 2))
                    {
                        *(source_val + 0x2c) = *data_11;
                        Render_DrawLightmapMesh_8(nullptr, nullptr, source_val);
                        *(source_val + 0x10) = *dest_1[1] - *(source_val + 0x1c);
                        *(source_val + 0x14) = *(dest_1[1] + 4) - *(source_val + 0x20);
                        long double fpu_val_7_22 = *(dest_1[1] + 8) - *(source_val + 0x24);
                        int data_21 = *(source_val + 8);
                        *(source_val + 0x2c) = temp_6;
                        *(&data_21 + 1) &= 0x7f;
                        *(source_val + 8) = data_21;
                        *(source_val + 0x18) = fpu_val_7_22;
                        return result;
                    }

                    *(source_val + 0x2c) = 0;
                    Render_DrawLightmapMesh_8(nullptr, nullptr, source_val);
                    *(source_val + 0x10) = *(dest_1[1] + *dest_1 * 0x28 - 0x28) - *(source_val + 0x1c);
                    *(source_val + 0x14) = *(dest_1[1] + *dest_1 * 0x28 - 0x24) - *(source_val + 0x20);
                    int data_17 = *(source_val + 8);
                    *(&data_17 + 1) &= 0x7f;
                    long double fpu_val_7_16 = *(dest_1[1] + *dest_1 * 0x28 - 0x20) - *(source_val + 0x24);
                    *(source_val + 8) = data_17;
                    *(source_val + 0x2c) = temp_6;
                    *(source_val + 0x18) = fpu_val_7_16;
                    return result;
                }
            }
        }
    }
    else
    {
        long double fpu_val_7_4 = *(source_val + 0x2c);
        long double f1_1 = 0.0f;
        fpu_val_7_4 - f1_1;

        if (*(&((fpu_val_7_4 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_4 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x41)
        {
            data_6 = *(source_val + 8);

            if (data_6 & 2)
                goto label_4d4c4a;
        }
    }

    int data_26 = *(source_val + 8);
    *(&data_26 + 1) &= 0x7f;
    *(source_val + 8) = data_26;
    return result;
}
