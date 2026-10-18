// IGI native provenance: Physics_EvaluateAreaCriteria_14 - 0x0050EDC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050EDC0-Physics_EvaluateAreaCriteria_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_EvaluateAreaCriteria_14.h"
void Physics_EvaluateAreaCriteria_14(float context)

{
    int entity_11 = g_bcaaa8;

    if (!entity_11)
        return;

    float flagP1 = context;
    int item_10_1 = *(flagP1 + 0x50);
    int item_10_2 = *(flagP1 + 0x4c);
    int item_10_3 = g_bcaac0;
    int* item_50_1 = entity_11 + ((g_bcaaa0 * item_10_1 + item_10_2) << 2);
    int context_1 = g_bcaac4;
    int item_24_1 = context_1;
    int temp_5 = g_bcaac8 + item_10_3;
    int item_28_1 = temp_5;
    int entity_3 = g_bcaacc + context_1;

    if (item_10_2 >= temp_5 || item_10_1 >= entity_3 || *(flagP1 + 0x6c) + item_10_2 < item_10_3)
        return;

    int item_10_8 = *(flagP1 + 0x70);

    if (item_10_8 + item_10_1 < context_1)
        return;

    long double fpu_val_7_9 = *(flagP1 + 0x54);
    int item_44_1 = 0;
    context = 1.0f - fpu_val_7_9;
    int item_10_9 = fpu_val_7_9 * *(flagP1 + 0x58) * 255.0f;
    int item_10_10 = fpu_val_7_9 * *(flagP1 + 0x5c) * 255.0f;
    long long item_10_11 = fpu_val_7_9 * *(flagP1 + 0x60) * 255.0f;
    int dest_6 = item_10_11;

    if (item_10_8 <= 0)
        return;

    int entity_7 = item_10_1;
    int item_40_1 = entity_7;
    bool flag_3_1;

    do
    {
        if (entity_7 >= context_1 && entity_7 < entity_3)
        {
            long double fpu_val_7_12 = 0.0f;
            long double f0_1 = *(flagP1 + 0x6c);
            fpu_val_7_12 - f0_1;
            int item_54_1 = 0;

            if (*(&((fpu_val_7_12 < f0_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_12, f0_1) ? 1 : 0) << 0xa
                | (fpu_val_7_12 == f0_1 ? 1 : 0) << 0xe) + 1) & 1)
            {
                int entity_8 = item_10_2;
                int* dest_7 = item_50_1;
                int item_48_1 = entity_8;
                long double fpu_val_7_19;
                long double f1_1;

                do
                {
                    if (entity_8 >= item_10_3 && entity_8 < temp_5)
                    {
                        int data_2 = *dest_7;
                        int item_1c_1 = 0;
                        int item_14_1 = 0;
                        *(&item_10_11 + 4) = 0;
                        item_10_11 = data_2 & 0xff;
                        int data_5 = item_10_9 + (data_2 >> 0x10) * context;
                        char entity_10 = -1;

                        if (data_5 <= 0xff)
                            entity_10 = data_5;

                        int source_1 = (data_2 >> 8) * context + item_10_10;

                        if (source_1 > 0xff)
                            source_1 = 0xff;

                        int data_7 = item_10_11 * context + dest_6;

                        if (data_7 > 0xff)
                            data_7 = 0xff;

                        int temp_14 = 0;
                        *(&temp_14 + 1) = entity_10;
                        entity_8 = item_48_1;
                        *dest_7 = (temp_14 | (source_1 & 0xffff)) << 8 | (data_7 & 0xffffff);
                        temp_5 = item_28_1;
                    }

                    dest_7 = &dest_7[1];
                    entity_8 += 1;
                    item_54_1 += 1;
                    item_48_1 = entity_8;
                    fpu_val_7_19 = item_54_1;
                    f1_1 = *(flagP1 + 0x6c);
                    fpu_val_7_19 - f1_1;
                } while (*(&((fpu_val_7_19 < f1_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_19, f1_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_19 == f1_1 ? 1 : 0) << 0xe) + 1) & 1);
                entity_7 = item_40_1;
                context_1 = item_24_1;
            }
        }

        item_50_1 = &item_50_1[g_bcaaa0];
        entity_7 += 1;
        flag_3_1 = item_44_1 + 1 < *(flagP1 + 0x70);
        item_44_1 += 1;
        item_40_1 = entity_7;
    } while (flag_3_1);
}
