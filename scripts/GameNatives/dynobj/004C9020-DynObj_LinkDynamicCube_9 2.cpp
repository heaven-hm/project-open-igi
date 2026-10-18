// IGI native provenance: DynObj_LinkDynamicCube_9 - 0x004C9020 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C9020-DynObj_LinkDynamicCube_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_LinkDynamicCube_9.h"
int DynObj_LinkDynamicCube_9(short* context, double* arg2, double* arg3, char arg4, int* arg5, int arg6)

{
    int data_val = g_ba2058;
    int item_72c;
    char item_71c;
    float item_6f0;
    float item_6ec;
    float item_6d4;
    int source_val;

    if (!data_val)
        source_val = item_72c;
    else
    {
        int data_2 = data_val * 0x30;
        item_6f0 = *(data_2 + 0xba2048);
        long double fpu_val_7_2 = *(data_2 + 0xba2050);
        source_val = (&g_ba2058)[data_val * 0xc];
        item_6d4 = (&g_ba2040)[data_val * 6];
        item_72c = source_val;
        item_6ec = fpu_val_7_2;
        item_71c = *(data_2 + 0xba203c) + 3;
    }

    float item_6d0 = 1.0f;
    short* dest_val = &context[4];
    short* context_1 = context[1] + dest_val;
    int data_5 = context[3] + context[2];

    if (arg6 & 0xfffffffc)
        item_6d0 = -1.0f;

    double item_6e8 = (0x40000000 >> (arg4 & 0x1f)) * 0.041666666666666664;
    void* item_730;
    int item_728;
    float item_718;
    double item_710;
    float item_700;
    void item_608;

    if (data_5 > 0)
    {
        item_730 = &item_608;
        int temp_3 = arg6 & 4;
        int entity_8 = arg6 & 1;
        int item_6c8_1 = temp_3;
        int item_6cc_1 = entity_8;
        int item_734_1 = data_5;

        while (true)
        {
            int data_6 = 0;
            int dest_1 = 0;
            data_6 = context_1[1];
            dest_1 = *context_1;
            int data_8 = (data_6 << 0x10) + dest_1;
            long double fpu_val_7_6 = ((data_8 >> 0x1a) - 0x18) * item_6e8;
            double item_710_2 = ((data_8 >> 0x14 & 0x3f) - 0x18) * item_6e8;
            double item_708_2 = ((data_8 >> 0xe & 0x3f) - 0x18) * item_6e8;

            if (temp_3)
                fpu_val_7_6 = -(fpu_val_7_6);

            if (entity_8)
            {
                item_700 = item_710_2;
                int item_6fc_1 = *(&item_710_2 + 4);

                if (!(arg6 & 2))
                {
                    item_710_2 = fpu_val_7_6;
                    fpu_val_7_6 = -(item_700);
                }
                else
                {
                    item_710_2 = -(fpu_val_7_6);
                    fpu_val_7_6 = item_700;
                }
            }
            else if (arg6 & 2)
            {
                fpu_val_7_6 = -(fpu_val_7_6);
                item_710_2 = -(item_710_2);
            }

            item_718 = fpu_val_7_6 + *arg2;
            item_710 = item_710_2 + arg2[1];
            double item_708_3 = item_708_2 + arg2[2];

            if (g_ba2058)
            {
                long double fpu_val_7_16 = (item_718 - item_6f0) * item_6d4;
                float item_738_4 = (item_710 - item_6ec) * item_6d4;
                item_728 = fpu_val_7_16;
                int temp_4 = item_728;
                item_728 = item_738_4;
                int data_16 = item_728;
                item_728 = data_16;
                long double fpu_val_7_17 = fpu_val_7_16 - temp_4;
                int entity_11 = (data_16 << (item_71c & 0x1f)) + data_16;
                long double fpu_val_6_15 = item_738_4 - item_728;
                int entity_13 = *(entity_11 + temp_4 + 1 + source_val);
                item_728 = *(entity_11 + temp_4 + source_val);
                long double fpu_val_5_1 = item_728;
                item_728 = entity_13;
                long double fpu_val_4_1 = item_728;
                void* dest_16 = ((data_16 + 1) << (item_71c & 0x1f)) + data_16 + 1 + temp_4 + 1;
                item_728 = *(dest_16 + source_val - 1);
                float item_738_5 = item_728;
                item_728 = *(dest_16 + source_val);
                temp_3 = item_6c8_1;
                long double fpu_val_5_2 = fpu_val_5_1 + (fpu_val_4_1 - fpu_val_5_1) * fpu_val_7_17;
                double item_708_4 = (
                    (fpu_val_7_17 * (item_728 - item_738_5) + item_738_5 - fpu_val_5_2) * fpu_val_6_15
                    + fpu_val_5_2 - 64.0) * 256.0f + item_708_3;
            }

            memcpy(item_730, &item_718, 0x18);
            context_1 = &context_1[2];
            bool flag_7_1 = item_734_1 != 1;
            item_730 += 0x18;
            item_734_1 -= 1;

            if (!flag_7_1)
                break;

            source_val = item_72c;
            entity_8 = item_6cc_1;
        }
    }

    int data_22 = 0;
    data_22 = dest_val[1];
    int temp_7 = 0;
    temp_7 = *dest_val;
    int item_71c_1 = 0;
    int source_3 = *context;
    void* data_24 = (data_22 << 0x10) + temp_7;
    int entity_16 = 0;
    entity_16 = *(&data_24 + 1);
    item_730 = data_24;
    int data_25 = 0;
    data_25 = *(&item_730 + 2);
    unsigned int temp_11 = data_24 * 0x18;
    int entity_18 = entity_16 * 0x18;
    int item_6f0_1 = 0;
    int data_27 = data_25 * 0x18;
    int item_6ec_1 = 0x43d00000;
    double item_6a4 =
        (*(&item_608 + temp_11) + *(&item_608 + entity_18) + *(&item_608 + data_27)) * 0.33333333333333331;
    void item_600;
    double item_69c_1 =
        (*(&item_600 + temp_11) + *(&item_600 + entity_18) + *(&item_600 + data_27)) * 0.33333333333333331;
    long long item_5f8[0xbe];
    double item_694_1 =
        (item_5f8[data_24 * 3] + item_5f8[entity_16 * 3] + item_5f8[data_25 * 3]) * 0.33333333333333331;

    if (source_3 > 0)
    {
        short* entity_19 = dest_val;
        int item_734_2 = source_3;
        short* item_72c_1 = entity_19;
        float item_6e0;
        double item_620;

        while (true)
        {
            int data_28 = 0;
            int temp_12 = 0;
            data_28 = entity_19[1];
            temp_12 = *entity_19;
            void* data_30 = (data_28 << 0x10) + temp_12;
            int temp_13 = 0;
            temp_13 = *(&data_30 + 1);
            item_730 = data_30;
            int temp_14 = 0;
            temp_14 = *(&item_730 + 2);
            unsigned int entity_23 = data_30 * 0x18;
            int data_33 = temp_13 * 0x18;
            int saved_target;
            double item_650 = *(&item_608 + data_33) - *(&saved_target + entity_23 + 0x140);
            double item_648_1 = *(&item_600 + data_33) - *(&saved_target + entity_23 + 0x148);
            int data_35 = temp_14 * 0x18;
            double item_640_1 = item_5f8[temp_13 * 3] - *(&saved_target + entity_23 + 0x150);
            double item_638 = *(&item_608 + data_35) - *(&saved_target + entity_23 + 0x140);
            double item_630_1 = *(&item_600 + data_35) - *(&saved_target + entity_23 + 0x148);
            double item_628_1 = item_5f8[temp_14 * 3] - *(&saved_target + entity_23 + 0x150);
            double item_668;
            Sys_CloseFileStream_11(&item_668, &item_650, &item_638);
            long double fpu_val_7_43 = item_668;
            long double f1_1 = 0.0;
            fpu_val_7_43 - f1_1;
            double item_660;
            double item_658;

            if (!(*(&((fpu_val_7_43 < f1_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_43, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_43 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            {
            label_4c94f4:
                Sys_FreeMemory_11(&item_668);
                item_718 = item_650;
                float item_714_1 = item_648_1;
                item_710 = item_640_1;
                float item_68c = item_638;
                float item_688_1 = item_630_1;
                float item_684_1 = item_628_1;
                item_700 = item_668;
                float item_6fc_2 = item_660;
                float item_6f8_1 = item_658;
                float item_680 = *arg3 - *(&saved_target + entity_23 + 0x140);
                float item_67c_1 = arg3[1] - *(&saved_target + entity_23 + 0x148);
                float item_678_1 = arg3[2] - *(&saved_target + entity_23 + 0x150);
                float item_674 = item_6a4 - *arg3;
                float item_670_1 = item_69c_1 - arg3[1];
                float item_66c_1 = item_694_1 - arg3[2];

                if (Sys_PopStack_10(&item_728, &item_680, &item_674, &item_700)
                    && Sys_CheckHeap_10(nullptr, nullptr, &item_728, &item_718, &item_68c, &item_700))
                {
                    double item_6bc = *(&saved_target + entity_23 + 0x140) - *arg3 + item_728;
                    float item_724;
                    double item_6b4_1 = *(&saved_target + entity_23 + 0x148) - arg3[1] + item_724;
                    float item_720;
                    long double fpu_val_7_75 = *(&saved_target + entity_23 + 0x150) - arg3[2] + item_720;
                    long double fpu_val_7_78 =
                        fpu_val_7_75 * fpu_val_7_75 + item_6b4_1 * item_6b4_1 + item_6bc * item_6bc;
                    long double fpu_val_6_27 = item_6f0_1;
                    fpu_val_6_27 - fpu_val_7_78;

                    if (!(*(&((fpu_val_6_27 < fpu_val_7_78 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_27, fpu_val_7_78) ? 1 : 0) << 0xa
                        | (fpu_val_6_27 == fpu_val_7_78 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                    {
                        *(&item_6e8 + 4) = item_6fc_2;
                        item_6f0_1 = fpu_val_7_78;
                        item_71c_1 = 1;
                        item_6e8 = item_700;
                        item_6e0 = item_6f8_1;
                        memcpy(&item_620, &item_6bc, 0x18);
                    }
                }
            }
            else
            {
                long double fpu_val_7_44 = item_660;
                long double f2_1 = 0.0;
                fpu_val_7_44 - f2_1;

                if (!(*(&((fpu_val_7_44 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_44, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_44 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_4c94f4;

                long double fpu_val_7_45 = item_658;
                long double f4_1 = 0.0;
                fpu_val_7_45 - f4_1;

                if (!(*(&((fpu_val_7_45 < f4_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_45, f4_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_45 == f4_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_4c94f4;
            }
            bool flag_5_1 = item_734_2 != 1;
            item_72c_1 = &item_72c_1[2];
            item_734_2 -= 1;

            if (!flag_5_1)
                break;

            entity_19 = item_72c_1;
        }

        if (item_71c_1)
        {
            double item_618;
            double item_610;
            long double fpu_val_7_84 =
                (item_6e0 * item_610 + *(&item_6e8 + 4) * item_618 + item_6e8 * item_620) * item_6d0;
            long double f5 = 0.0;
            fpu_val_7_84 - f5;

            if (*(&((fpu_val_7_84 < f5 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_84, f5) ? 1 : 0) << 0xa
                | (fpu_val_7_84 == f5 ? 1 : 0) << 0xe) + 1) & 0x41)
            {
                *arg5 = 0;
                return 1;
            }

            *arg5 = 1;
            return 1;
        }
    }

    *arg5 = 0;
    return 1;
}
