// IGI native provenance: Render_DrawLightmapMesh_8 - 0x004D4840 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D4840-Render_DrawLightmapMesh_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D4840-Render_DrawLightmapMesh_8.hpp"
int Render_DrawLightmapMesh_8(int* context, int* arg2, float arg3)

{
    float temp_4 = arg3;
    int* data_1 = *(*(temp_4 + 0xc) + 8);
    int dest_val = *data_1;
    int* source_val = data_1[1];
    int* data_2;
    int* entity_1;

    if (dest_val != 1)
    {
        float data_3 = *(temp_4 + 0x2c);
        int entity_2 = 0;

        if (dest_val > 0)
        {
            int* context_1 = &source_val[3];

            do
            {
                arg3 = *context_1;
                long double fpu_val_7_1 = arg3;
                long double f2_1 = data_3;
                fpu_val_7_1 - f2_1;

                if (!(*(&((fpu_val_7_1 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_1, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_1 == f2_1 ? 1 : 0) << 0xe) + 1) & 1))
                    break;

                entity_2 += 1;
                context_1 = &context_1[0xa];
            } while (entity_2 < dest_val);
        }

        long double fpu_val_7_2 = data_3;
        long double f1_1 = arg3;
        fpu_val_7_2 - f1_1;
        float item_4c_1;
        float item_48_1;
        float item_44_1;

        if (*(&((fpu_val_7_2 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_2 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40)
        {
            int entity_3 = entity_2 * 5;
            void* data_7 = &source_val[entity_3 * 2];
            item_4c_1 = source_val[entity_3 * 2];
            item_48_1 = *(data_7 + 4);
            item_44_1 = *(data_7 + 8);
        }
        else if (entity_2 != dest_val)
        {
            long double fpu_val_7_3 = arg3;
            int* data_12 = &source_val[entity_2 * 0xa];
            long double fpu_val_7_8;

            if (!g_a54678)
            {
                long double fpu_val_7_10 = (fpu_val_7_3 - data_3) / (arg3 - data_12[-7]);
                item_4c_1 = (data_12[-0xa] - *data_12) * fpu_val_7_10 + *data_12;
                item_48_1 = (data_12[-9] - data_12[1]) * fpu_val_7_10 + data_12[1];
                fpu_val_7_8 = fpu_val_7_10 * (data_12[-8] - data_12[2]) + data_12[2];
            }
            else
            {
                long double fpu_val_7_4 = fpu_val_7_3 - data_12[-7];
                long double fpu_val_6_4 = 1.0f - (arg3 - data_3) / fpu_val_7_4;
                long double fpu_val_5_2 = fpu_val_6_4 * fpu_val_6_4;
                arg3 = fpu_val_5_2;
                long double fpu_val_5_3 = fpu_val_5_2 * fpu_val_6_4;
                float item_50_2 = fpu_val_5_3;
                long double fpu_val_5_5 = fpu_val_5_3 + fpu_val_5_3 - arg3 * 3.0f;
                float item_10_1 = fpu_val_5_5 + 1.0f;
                float item_c_1 = -(fpu_val_5_5);
                long double fpu_val_5_7 = arg3;
                float item_8_1 = fpu_val_6_4 - (fpu_val_5_7 + fpu_val_5_7) + item_50_2;
                float item_4_1 = arg3 - item_50_2;
                long double fpu_val_6_22 = *data_12 * item_c_1 + data_12[7] * fpu_val_7_4 * item_4_1
                    + data_12[-0xa] * item_10_1 + data_12[-6] * fpu_val_7_4 * item_8_1;
                item_4c_1 = fpu_val_6_22;
                item_48_1 = data_12[1] * item_c_1 + data_12[-9] * item_10_1
                    + data_12[8] * fpu_val_7_4 * item_4_1 + data_12[-5] * fpu_val_7_4 * item_8_1;
                fpu_val_7_8 = data_12[2] * item_c_1 + data_12[-8] * item_10_1
                    + fpu_val_7_4 * data_12[9] * item_4_1 + data_12[-4] * fpu_val_7_4 * item_8_1;
            }

            item_44_1 = fpu_val_7_8;
        }
        else
        {
            int entity_6 = dest_val * 5;
            item_4c_1 = source_val[entity_6 * 2 - 0xa];
            item_48_1 = source_val[entity_6 * 2 - 9];
            item_44_1 = source_val[entity_6 * 2 - 8];
        }

        data_2 = temp_4 + 0x10;
        entity_1 = temp_4 + 0x1c;
        *entity_1 = item_4c_1 - *(temp_4 + 0x10);
        *(temp_4 + 0x20) = item_48_1 - *(temp_4 + 0x14);
        *(temp_4 + 0x24) = item_44_1 - *(temp_4 + 0x18);
        *data_2 = item_4c_1;
        data_2[1] = item_48_1;
        data_2[2] = item_44_1;
    }
    else
    {
        data_2 = temp_4 + 0x10;
        *data_2 = *source_val;
        data_2[1] = source_val[1];
        int source_1 = source_val[2];
        *(temp_4 + 0x1c) = 0;
        *(temp_4 + 0x20) = 0;
        data_2[2] = source_1;
        entity_1 = temp_4 + 0x1c;
        *(temp_4 + 0x24) = 0;
    }

    if (context)
    {
        *context = *data_2;
        context[1] = data_2[1];
        context[2] = data_2[2];
    }

    if (arg2)
    {
        *arg2 = *entity_1;
        arg2[1] = entity_1[1];
        arg2[2] = entity_1[2];
    }

    return 1;
}
