// IGI native provenance: Render_ComputeNormals_8 - 0x004D4DC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D4DC0-Render_ComputeNormals_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D4DC0-Render_ComputeNormals_8.hpp"
int Render_ComputeNormals_8(int context, int arg2, int* arg3, void* arg4, float arg5, float arg6)

{
    void* context_val = arg4;
    int dest_val = 0;

    if (context_val)
    {
        int item_8_1 = 1;
        long double fpu_val_7_1 = arg6;
        long double f0_1 = arg5;
        fpu_val_7_1 - f0_1;
        int* data_2 = *(*(context_val + 0xc) + 0x10);
        int flagP1 = *data_2;
        int item_4_1 = flagP1;

        if (!(*(&((fpu_val_7_1 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f0_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f0_1 ? 1 : 0) << 0xe) + 1) & 1))
            item_8_1 = 0;

        long double fpu_val_7_2 = arg5;
        long double f1_1 = arg6;
        fpu_val_7_2 - f1_1;
        float item_c_1;

        item_c_1 = !(*(&((fpu_val_7_2 < f1_1 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_2, f1_1) ? 1 : 0) << 0xa | (fpu_val_7_2 == f1_1 ? 1 : 0) << 0xe) + 1)
            & 1) ? arg6 : arg5;

        long double fpu_val_7_3 = arg5;
        long double f2_1 = arg6;
        fpu_val_7_3 - f2_1;

        if (*(&((fpu_val_7_3 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_3 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x41)
            arg5 = arg6;

        if (flagP1 > 0)
        {
            int source_val = arg2;
            float* temp_3 = data_2[1] + 8;
            int* entity_4 = context + source_val * 0x1c + 8;

            do
            {
                long double fpu_val_7_4 = *temp_3;
                long double f3_1 = arg5;
                fpu_val_7_4 - f3_1;

                if (!(*(&((fpu_val_7_4 < f3_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_4, f3_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_4 == f3_1 ? 1 : 0) << 0xe) + 1) & 0x41))
                    break;

                long double fpu_val_7_5 = item_c_1;
                long double f4_1 = arg5;
                fpu_val_7_5 - f4_1;
                long double fpu_val_7_6;
                long double f6_1;

                if (*(&((fpu_val_7_5 < f4_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_5, f4_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_5 == f4_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                {
                    fpu_val_7_6 = *temp_3;
                    f6_1 = item_c_1;
                    fpu_val_7_6 - f6_1;
                }

                if (*(&((fpu_val_7_5 < f4_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_5, f4_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_5 == f4_1 ? 1 : 0) << 0xe) + 1) & 0x40 && *(&(
                    (fpu_val_7_6 < f6_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f6_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_6 == f6_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                {
                label_4d4ebb:
                    source_val += 1;
                    entity_4[-2] = *arg3;
                    entity_4[-1] = temp_3[-2];
                    *entity_4 = *(context_val + 4);
                    entity_4[1] = *temp_3;
                    int* context_1 = &entity_4[2];
                    entity_4 = &entity_4[7];
                    *context_1 = temp_3[1];
                    context_1[1] = temp_3[2];
                    flagP1 = item_4_1;
                    context_1[2] = temp_3[3];
                    context_val = arg4;
                }
                else
                {
                    long double fpu_val_7_7 = *temp_3;
                    long double f5_1 = item_c_1;
                    fpu_val_7_7 - f5_1;
                    short data_7 = (fpu_val_7_7 < f5_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_7, f5_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_7 == f5_1 ? 1 : 0) << 0xe;

                    if (!item_8_1)
                    {
                        if (!(*(&data_7 + 1) & 0x41))
                            goto label_4d4ebb;
                    }
                    else if (!(*(&data_7 + 1) & 1))
                    {
                        long double fpu_val_7_8 = *temp_3;
                        long double f7_1 = arg5;
                        fpu_val_7_8 - f7_1;

                        if (*(&((fpu_val_7_8 < f7_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_8, f7_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_8 == f7_1 ? 1 : 0) << 0xe) + 1) & 1)
                            goto label_4d4ebb;
                    }
                }

                dest_val += 1;
                temp_3 = &temp_3[6];
            } while (dest_val < flagP1);

            return source_val;
        }
    }

    return arg2;
}
