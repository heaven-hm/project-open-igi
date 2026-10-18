// IGI native provenance: Render_ClipMesh_14 - 0x004DD4B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DD4B0-Render_ClipMesh_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_ClipMesh_14.h"
int* Render_ClipMesh_14(void* context, float* arg2, int* arg3, int arg4)

{
    int temp_5 = g_a5e340;
    int context_val = g_a5e348;
    int* result_1 = g_a5e33c;
    int dest_val = 0;

    if (temp_5 > 0)
    {
        int* result = result_1;

        do
        {
            float* entity_3 = *result * context_val + g_a5e338;

            if (entity_3[6] == context)
            {
                long double fpu7 = *entity_3;
                long double f0 = *arg2;
                fpu7 - f0;
                result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
                    | (fpu7 == f0 ? 1 : 0) << 0xe;
                long double fpu_val_7_1;

                if (!(*(&result + 1) & 1))
                    fpu_val_7_1 = *arg2;
                else
                    fpu_val_7_1 = *entity_3;

                *entity_3 = fpu_val_7_1;
                long double fpu_val_7_2 = entity_3[1];
                long double fpu6 = arg2[1];
                fpu6 - fpu_val_7_2;
                result = (fpu6 < fpu_val_7_2 ? 1 : 0) << 8
                    | (FCMP_UO(fpu6, fpu_val_7_2) ? 1 : 0) << 0xa
                    | (fpu6 == fpu_val_7_2 ? 1 : 0) << 0xe;
                long double fpu_val_7_3;

                if (*(&result + 1) & 0x41)
                    fpu_val_7_3 = arg2[1];
                else
                    fpu_val_7_3 = entity_3[1];

                entity_3[1] = fpu_val_7_3;
                long double fpu_val_7_4 = entity_3[2];
                long double f1 = *arg3;
                fpu_val_7_4 - f1;
                result = (fpu_val_7_4 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f1) ? 1 : 0) << 0xa
                    | (fpu_val_7_4 == f1 ? 1 : 0) << 0xe;
                long double fpu_val_7_5;

                if (*(&result + 1) & 0x41)
                    fpu_val_7_5 = *arg3;
                else
                    fpu_val_7_5 = entity_3[2];

                entity_3[2] = fpu_val_7_5;
                long double fpu_val_7_6 = entity_3[3];
                long double fpu_val_6_1 = arg3[1];
                fpu_val_6_1 - fpu_val_7_6;
                result = (fpu_val_6_1 < fpu_val_7_6 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_1, fpu_val_7_6) ? 1 : 0) << 0xa
                    | (fpu_val_6_1 == fpu_val_7_6 ? 1 : 0) << 0xe;

                if (!(*(&result + 1) & 1))
                {
                    entity_3[3] = arg3[1];
                    return result;
                }

                entity_3[3] = entity_3[3];
                return result;
            }

            dest_val += 1;
            result = &result[1];
        } while (dest_val < temp_5);
    }

    int* data_3 = result_1[temp_5] * context_val + g_a5e338;
    *(g_a5e34c + data_3) = temp_5;
    int* entity_6 = arg2;
    g_a5e340 += 1;
    data_3[6] = context;
    data_3[5] = arg4;
    *data_3 = *entity_6;
    data_3[1] = entity_6[1];
    data_3[2] = *arg3;
    data_3[3] = arg3[1];
    data_3[4] = g_a5e358 + 1;
    data_3[7] = 0;
    return Render_DrawBoneMesh_15(context, entity_6, arg3, arg4);
}
