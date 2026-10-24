// IGI native provenance: DynObj_LookupHashItem_11 - 0x004CD300 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004CD300-DynObj_LookupHashItem_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_LookupHashItem_11.h"
int DynObj_LookupHashItem_11(short* context, double* arg2, double* arg3, double* arg4, int arg5, char arg6, int* arg7, int* arg8, int arg9, float* arg10)

{
    int data_val = g_ba2058;
    int item_72c;
    float item_6e8;
    char item_6e4;
    float item_6e0;
    float item_6dc;

    if (data_val)
    {
        int data_2 = data_val * 0x30;
        item_6e0 = *(data_2 + 0xba2048);
        item_6e8 = (&g_ba2040)[data_val * 6];
        item_72c = (&g_ba2058)[data_val * 0xc];
        item_6dc = *(data_2 + 0xba2050);
        item_6e4 = *(data_2 + 0xba203c) + 3;
    }

    float item_734 = 1.0f;
    int* context_val = &context[4];
    int data_5 = context[3] + context[2];
    int* temp_3 = context[1] + context_val;
    int entity_7 = arg9 & 4;
    int* item_738 = temp_3;

    if (entity_7)
        item_734 = -1.0f;

    double item_638;
    memcpy(&item_638, arg3, 0x18);
    double item_6b0;
    memcpy(&item_6b0, arg4, 0x18);
    double item_728 = (0x40000000 >> (arg6 & 0x1f)) * 0.041666666666666664;
    float item_748;
    float item_744;
    double item_718;
    void item_608;

    if (data_5 > 0)
    {
        int entity_10 = arg9 & 1;
        void* item_730 = &item_608;
        int item_6f8_1 = entity_10;
        int item_74c_1 = data_5;

        while (true)
        {
            int data_6 = *temp_3;
            long double fpu_val_7_6 = ((data_6 >> 0x1a & 0x3f) - 0x18) * item_728;
            double item_710_2 = ((data_6 >> 0x14 & 0x3f) - 0x18) * item_728;
            double item_708_2 = ((data_6 >> 0xe & 0x3f) - 0x18) * item_728;

            if (entity_7)
                fpu_val_7_6 = -(fpu_val_7_6);

            if (entity_10)
            {
                item_748 = item_710_2;
                item_744 = *(&item_710_2 + 4);

                if (!(arg9 & 2))
                {
                    item_710_2 = fpu_val_7_6;
                    fpu_val_7_6 = -(item_748);
                }
                else
                {
                    item_710_2 = -(fpu_val_7_6);
                    fpu_val_7_6 = item_748;
                }
            }
            else if (arg9 & 2)
            {
                fpu_val_7_6 = -(fpu_val_7_6);
                item_710_2 = -(item_710_2);
            }

            item_718 = fpu_val_7_6 + *arg2;

            if (g_ba2058)
            {
                long double fpu_val_7_16 = (item_718 - item_6e0) * item_6e8;
                float item_758_4 = (item_710_2 + arg2[1] - item_6dc) * item_6e8;
                int item_754;
                item_754 = fpu_val_7_16;
                int temp_4 = item_754;
                item_754 = item_758_4;
                long double fpu_val_7_17 = fpu_val_7_16 - temp_4;
                int entity_13 = (item_754 << (item_6e4 & 0x1f)) + item_754;
                long double fpu_val_6_15 = item_758_4 - item_754;
                long double fpu_val_5_1 = *(entity_13 + temp_4 + item_72c);
                void* dest_6 = ((item_754 + 1) << (item_6e4 & 0x1f)) + item_754 + 1 + temp_4 + 1;
                float item_758_5 = *(dest_6 + item_72c - 1);
                item_754 = *(dest_6 + item_72c);
                temp_3 = item_738;
                long double fpu_val_5_2 =
                    fpu_val_5_1 + (*(entity_13 + temp_4 + 1 + item_72c) - fpu_val_5_1) * fpu_val_7_17;
                double item_708_4 = (
                    (fpu_val_7_17 * (item_754 - item_758_5) + item_758_5 - fpu_val_5_2) * fpu_val_6_15
                    + fpu_val_5_2 - 64.0) * 256.0f + item_708_2 + arg2[2];
            }

            memcpy(item_730, &item_718, 0x18);
            temp_3 = &temp_3[1];
            bool flag_8_1 = item_74c_1 != 1;
            item_730 += 0x18;
            item_738 = temp_3;
            item_74c_1 -= 1;

            if (!flag_8_1)
                break;

            entity_10 = item_6f8_1;
        }
    }

    float item_698 = item_6b0 - item_638;
    double item_6a8;
    double item_630;
    float item_694 = item_6a8 - item_630;
    int data_21 = *context;
    double item_6a0;
    double item_628;
    float item_690 = item_6a0 - item_628;
    double item_620[0x3];
    memcpy(&item_620, arg4, 0x18);
    int item_738_1 = 0;

    if (data_21 > 0)
    {
        int item_74c_2 = data_21;
        float item_740;
        bool c1;

        while (true)
        {
            int24_t data_22 = *context_val;
            int temp_7 = 0;
            temp_7 = *(&data_22 + 1);
            int temp_8 = 0;
            unsigned int entity_21 = data_22 * 0x18;
            int data_25 = temp_7 * 0x18;
            temp_8 = *(&data_22 + 2);
            int saved_target;
            double item_668 = *(&item_608 + data_25) - *(&saved_target + entity_21 + 0x160);
            void item_600;
            double item_660_1 = *(&item_600 + data_25) - *(&saved_target + entity_21 + 0x168);
            int data_27 = temp_8 * 0x18;
            long long item_5f8[0xbe];
            double item_658_1 = item_5f8[temp_7 * 3] - *(&saved_target + entity_21 + 0x170);
            double item_650 = *(&item_608 + data_27) - *(&saved_target + entity_21 + 0x160);
            double item_648_1 = *(&item_600 + data_27) - *(&saved_target + entity_21 + 0x168);
            double item_640_1 = item_5f8[temp_8 * 3] - *(&saved_target + entity_21 + 0x170);
            Sys_CloseFileStream_11(&item_6b0, &item_668, &item_650);
            Sys_FreeMemory_11(&item_6b0);
            float item_680 = item_668;
            float item_67c_1 = item_660_1;
            float item_678_1 = item_658_1;
            float item_674 = item_650;
            float item_670_1 = item_648_1;
            float item_66c_1 = item_640_1;
            item_728 = item_6b0;
            *(&item_728 + 4) = item_6a8;
            float item_720_1 = item_6a0;
            float item_68c = item_638 - *(&saved_target + entity_21 + 0x160);
            float item_688_1 = item_630 - *(&saved_target + entity_21 + 0x168);
            float item_684_1 = item_628 - *(&saved_target + entity_21 + 0x170);
            float item_6bc;

            if (Sys_PopStack_10(&item_6bc, &item_68c, &item_698, &item_728)
                && Sys_CheckHeap_10(nullptr, nullptr, &item_6bc, &item_680, &item_674, &item_728))
            {
                double item_6d4 = item_6bc + *(&saved_target + entity_21 + 0x160);
                float item_6b8;
                double item_6cc = item_6b8 + *(&saved_target + entity_21 + 0x168);
                float item_6b4;
                double item_6c4_1 = item_6b4 + *(&saved_target + entity_21 + 0x170);
                item_718 = *arg3 - *arg4;
                long double fpu_val_7_65 = arg3[1] - arg4[1];
                long double fpu_val_6_18 = arg3[2] - arg4[2];
                long double fpu_val_5_11 = item_718;
                long double f2_1 = 0.0f;
                fpu_val_5_11 - f2_1;

                if (*(&((fpu_val_5_11 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_5_11, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_5_11 == f2_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
                    fpu_val_5_11 = -(fpu_val_5_11);

                long double f4_1 = 0.0f;
                fpu_val_7_65 - f4_1;

                if (*(&((fpu_val_7_65 < f4_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_65, f4_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_65 == f4_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
                    fpu_val_7_65 = -(fpu_val_7_65);

                long double f6_1 = 0.0f;
                fpu_val_6_18 - f6_1;

                if (*(&((fpu_val_6_18 < f6_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_18, f6_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_18 == f6_1 ? 1 : 0) << 0xe | 0x2800) + 1) & 1)
                    fpu_val_6_18 = -(fpu_val_6_18);

                fpu_val_5_11 - fpu_val_7_65;

                if (!(*(&((fpu_val_5_11 < fpu_val_7_65 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_5_11, fpu_val_7_65) ? 1 : 0) << 0xa
                        | (fpu_val_5_11 == fpu_val_7_65 ? 1 : 0) << 0xe | 0x2800) + 1) & 0x41))
                    fpu_val_5_11 - fpu_val_6_18;

                if (*(&((fpu_val_5_11 < fpu_val_7_65 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_5_11, fpu_val_7_65) ? 1 : 0) << 0xa
                    | (fpu_val_5_11 == fpu_val_7_65 ? 1 : 0) << 0xe | 0x2800) + 1) & 0x41 || *(&(
                    (fpu_val_5_11 < fpu_val_6_18 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_5_11, fpu_val_6_18) ? 1 : 0) << 0xa
                    | (fpu_val_5_11 == fpu_val_6_18 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41)
                {
                    fpu_val_7_65 - fpu_val_6_18;

                    if (*(&((fpu_val_7_65 < fpu_val_6_18 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_65, fpu_val_6_18) ? 1 : 0) << 0xa
                        | (fpu_val_7_65 == fpu_val_6_18 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
                    {
                        long double fpu_val_7_78 = arg3[2];
                        long double f8_1 = arg4[2];
                        fpu_val_7_78 - f8_1;
                        int data_34;
                        int temp_11;
                        long double fpu_val_7_79;

                        if (*(&((fpu_val_7_78 < f8_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_78, f8_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_78 == f8_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                        {
                            fpu_val_7_79 = arg3[2];
                            temp_11 = arg4[2];
                            data_34 = *(arg4 + 0x14);
                        }
                        else
                        {
                            fpu_val_7_79 = arg4[2];
                            temp_11 = arg3[2];
                            data_34 = *(arg3 + 0x14);
                        }

                        long double fpu_val_6_24 = item_6c4_1;
                        fpu_val_6_24 - fpu_val_7_79;
                        c1 = false;
                        int item_6f4_1 = data_34;

                        if (!(*(&((fpu_val_6_24 < fpu_val_7_79 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_6_24, fpu_val_7_79) ? 1 : 0) << 0xa
                            | (fpu_val_6_24 == fpu_val_7_79 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
                        {
                            long double fpu_val_7_81 = item_6c4_1;
                            long double f11_1 = temp_11;
                            fpu_val_7_81 - f11_1;
                            c1 = false;

                            if (*(&((fpu_val_7_81 < f11_1 ? 1 : 0) << 8
                                    | (FCMP_UO(fpu_val_7_81, f11_1) ? 1 : 0) << 0xa
                                    | (fpu_val_7_81 == f11_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                                goto label_4cd9ed;
                        }
                    }
                    else
                    {
                        long double fpu_val_7_74 = arg3[1];
                        long double f9_1 = arg4[1];
                        fpu_val_7_74 - f9_1;
                        int item_700_1;
                        long double fpu_val_7_75;

                        if (*(&((fpu_val_7_74 < f9_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_74, f9_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_74 == f9_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                        {
                            fpu_val_7_75 = arg3[1];
                            item_700_1 = arg4[1];
                            int item_6fc_2 = *(arg4 + 0xc);
                        }
                        else
                        {
                            fpu_val_7_75 = arg4[1];
                            item_700_1 = arg3[1];
                            int item_6fc_1 = *(arg3 + 0xc);
                        }

                        long double fpu_val_6_23 = item_6cc;
                        fpu_val_6_23 - fpu_val_7_75;
                        c1 = false;

                        if (!(*(&((fpu_val_6_23 < fpu_val_7_75 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_6_23, fpu_val_7_75) ? 1 : 0) << 0xa
                            | (fpu_val_6_23 == fpu_val_7_75 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
                        {
                            long double fpu_val_7_77 = item_6cc;
                            long double f12_1 = item_700_1;
                            fpu_val_7_77 - f12_1;
                            c1 = false;

                            if (*(&((fpu_val_7_77 < f12_1 ? 1 : 0) << 8
                                    | (FCMP_UO(fpu_val_7_77, f12_1) ? 1 : 0) << 0xa
                                    | (fpu_val_7_77 == f12_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                                goto label_4cd9ed;
                        }
                    }
                }
                else
                {
                    long double fpu_val_7_68 = *arg3;
                    long double f7_1 = *arg4;
                    fpu_val_7_68 - f7_1;
                    int item_6f0_2;
                    long double fpu_val_7_69;
                    int item_6ec;

                    if (*(&((fpu_val_7_68 < f7_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_68, f7_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_68 == f7_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                    {
                        fpu_val_7_69 = *arg3;
                        item_6f0_2 = *arg4;
                        item_6ec = *(arg4 + 4);
                    }
                    else
                    {
                        fpu_val_7_69 = *arg4;
                        item_6f0_2 = *arg3;
                        item_6ec = *(arg3 + 4);
                    }
                    long double fpu_val_6_21 = item_6d4;
                    fpu_val_6_21 - fpu_val_7_69;
                    c1 = false;

                    if (!(*(&((fpu_val_6_21 < fpu_val_7_69 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_21, fpu_val_7_69) ? 1 : 0) << 0xa
                        | (fpu_val_6_21 == fpu_val_7_69 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
                    {
                        long double fpu_val_7_71 = item_6d4;
                        long double f10_1 = item_6f0_2;
                        fpu_val_7_71 - f10_1;
                        c1 = false;

                        if (*(&((fpu_val_7_71 < f10_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_71, f10_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_71 == f10_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                        {
                        label_4cd9ed:
                            item_6e0 = Sys_PushStack_11(arg3, &item_6d4);
                            long double fp0_2 = Sys_PushStack_11(arg3, &item_620);
                            long double f13_1 = item_6e0;
                            fp0_2 - f13_1;
                            c1 = false;

                            if (!(*(&((fp0_2 < f13_1 ? 1 : 0) << 8
                                | (FCMP_UO(fp0_2, f13_1) ? 1 : 0) << 0xa
                                | (fp0_2 == f13_1 ? 1 : 0) << 0xe) + 1) & 0x41))
                            {
                                float data_38 = item_728;
                                item_738_1 = 1;
                                memcpy(&item_620, &item_6d4, 0x18);
                                item_748 = data_38;
                                item_744 = *(&item_728 + 4);
                                item_740 = item_720_1;
                            }
                        }
                    }
                }
            }

            bool flag_4_1 = item_74c_2 != 1;
            item_74c_2 -= 1;

            if (!flag_4_1)
                break;

            context_val = &context_val[1];
        }

        if (item_738_1)
        {
            float item_748_1 = item_748 * item_734;
            float item_744_1 = item_744 * item_734;
            long double fpu_val_4_13 = item_740 * item_734;
            long double fpu_val_4_16 = fpu_val_4_13 * (arg4[2] - arg3[2])
                + item_744_1 * (arg4[1] - arg3[1]) + item_748_1 * (*arg4 - *arg3);
            long double f3 = 0.0f;
            fpu_val_4_16 - f3;
            int data_44;

            data_44 = !(*(&((fpu_val_4_16 < f3 ? 1 : 0) << 8 | (c1 ? 1 : 0) << 9
                | (FCMP_UO(fpu_val_4_16, f3) ? 1 : 0) << 0xa | (fpu_val_4_16 == f3 ? 1 : 0) << 0xe
                | 0x3000) + 1) & 1) ? 0 : 1;

            *arg7 = data_44;
            *arg8 = data_44 ^ 1;
            *arg10 = item_748_1;
            arg10[1] = item_744_1;
            arg10[2] = fpu_val_4_13;
            memcpy(arg5, &item_620, 0x18);
            return 1;
        }
    }

    return 0;
}
