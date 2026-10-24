// IGI native provenance: World_TraceBatch - 0x004CDB20 - CFG score 87.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CDB20-World_TraceBatch.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "World_TraceBatch.h"
int* World_TraceBatch(int context, int* arg2, double* arg3, double* arg4, int* arg5, char arg6, double* arg7, int arg8, int arg9)

{
    int* result = arg5;
    int item_8 = 1;

    if (result)
    {
        int source_2 = g_a4438c + 1;
        g_a44390 += result;
        double* temp_3 = arg3;
        g_a4438c = source_2;
        double item_3c;
        memcpy(&item_3c, temp_3, 0x18);
        double item_24;
        memcpy(&item_24, temp_3, 0x18);
        double item_34;
        double item_1c;
        double item_14;
        double* context_2;
        long double fpu_val_7_1;
        double item_2c;

        if (result <= 0)
        {
            fpu_val_7_1 = item_2c;
            context_2 = arg4;
        }
        else
        {
            int* result_2 = result;
            fpu_val_7_1 = item_2c;
            double* dest_1 = &arg2[2];
            arg3 = dest_1;

            while (true)
            {
                long double fpu_val_6_1 = *temp_3;
                long double f0_1 = *arg4;
                fpu_val_6_1 - f0_1;
                long double fpu_val_6_2;
                long double f1_1;

                if (*(&((fpu_val_6_1 < f0_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_1, f0_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_1 == f0_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40)
                {
                    fpu_val_6_2 = temp_3[1];
                    f1_1 = arg4[1];
                    fpu_val_6_2 - f1_1;
                }

                if (!(*(&((fpu_val_6_1 < f0_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_1, f0_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_1 == f0_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40) || !(*(&(
                        (fpu_val_6_2 < f1_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_2, f1_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_2 == f1_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                    item_8 = 0;

                long double fpu_val_6_3 = *temp_3;
                long double f2_1 = item_3c;
                fpu_val_6_3 - f2_1;

                if (*(&((fpu_val_6_3 < f2_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_3, f2_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_3 == f2_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
                {
                    long double fpu_val_6_4 = *temp_3;
                    long double f3_1 = item_24;
                    fpu_val_6_4 - f3_1;

                    if (*(&((fpu_val_6_4 < f3_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_4, f3_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_4 == f3_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        int entity_3 = *(temp_3 + 4);
                        item_24 = *temp_3;
                        *(&item_24 + 4) = entity_3;
                    }
                }
                else
                {
                    int entity_2 = *(temp_3 + 4);
                    item_3c = *temp_3;
                    *(&item_3c + 4) = entity_2;
                }

                long double fpu_val_6_5 = temp_3[1];
                long double f4_1 = item_34;
                fpu_val_6_5 - f4_1;

                if (*(&((fpu_val_6_5 < f4_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_5, f4_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_5 == f4_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
                {
                    long double fpu_val_6_6 = temp_3[1];
                    long double f5_1 = item_1c;
                    fpu_val_6_6 - f5_1;

                    if (*(&((fpu_val_6_6 < f5_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_6, f5_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_6 == f5_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        item_1c = temp_3[1];
                        *(&item_1c + 4) = *(temp_3 + 0xc);
                    }
                }
                else
                {
                    item_34 = temp_3[1];
                    *(&item_34 + 4) = *(temp_3 + 0xc);
                }

                long double f7_1 = temp_3[2];
                fpu_val_7_1 - f7_1;

                if (!(*(&((fpu_val_7_1 < f7_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_1, f7_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_1 == f7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
                {
                    long double fpu_val_6_7 = temp_3[2];
                    long double f9_1 = item_14;
                    fpu_val_6_7 - f9_1;

                    if (*(&((fpu_val_6_7 < f9_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_7, f9_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_7 == f9_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        item_14 = temp_3[2];
                        *(&item_14 + 4) = *(temp_3 + 0x14);
                    }
                }
                else
                    fpu_val_7_1 = temp_3[2];

                long double fpu_val_6_8 = *arg4;
                long double f10_1 = item_3c;
                fpu_val_6_8 - f10_1;

                if (*(&((fpu_val_6_8 < f10_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_8, f10_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_8 == f10_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
                {
                    long double fpu_val_6_9 = *arg4;
                    long double f11_1 = item_24;
                    fpu_val_6_9 - f11_1;

                    if (*(&((fpu_val_6_9 < f11_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_9, f11_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_9 == f11_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        int entity_8 = *(arg4 + 4);
                        item_24 = *arg4;
                        *(&item_24 + 4) = entity_8;
                    }
                }
                else
                {
                    int entity_7 = *(arg4 + 4);
                    item_3c = *arg4;
                    *(&item_3c + 4) = entity_7;
                }

                long double fpu_val_6_10 = arg4[1];
                long double f12_1 = item_34;
                fpu_val_6_10 - f12_1;

                if (*(&((fpu_val_6_10 < f12_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_10, f12_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_10 == f12_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
                {
                    long double fpu_val_6_11 = arg4[1];
                    long double f13_1 = item_1c;
                    fpu_val_6_11 - f13_1;

                    if (*(&((fpu_val_6_11 < f13_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_11, f13_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_11 == f13_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        item_1c = arg4[1];
                        *(&item_1c + 4) = *(arg4 + 0xc);
                    }
                }
                else
                {
                    item_34 = arg4[1];
                    *(&item_34 + 4) = *(arg4 + 0xc);
                }

                long double f14_1 = arg4[2];
                fpu_val_7_1 - f14_1;

                if (!(*(&((fpu_val_7_1 < f14_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_1, f14_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_1 == f14_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
                {
                    long double fpu_val_6_12 = arg4[2];
                    long double f15_1 = item_14;
                    fpu_val_6_12 - f15_1;

                    if (*(&((fpu_val_6_12 < f15_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_6_12, f15_1) ? 1 : 0) << 0xa
                        | (fpu_val_6_12 == f15_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        item_14 = arg4[2];
                        *(&item_14 + 4) = *(arg4 + 0x14);
                    }
                }
                else
                    fpu_val_7_1 = arg4[2];

                dest_1[0xa] = 0;
                *arg2 = 0;
                memcpy(dest_1, arg4, 0x18);
                temp_3 = &temp_3[3];
                context_2 = &arg4[3];
                bool flag_4_1 = result_2 != 1;
                arg4 = context_2;
                arg2 = &arg2[0x1c];
                arg3 = &arg3[0xe];
                result_2 -= 1;

                if (!flag_4_1)
                    break;

                dest_1 = arg3;
            }

            result = arg5;
        }
        int entity_15 = result * 0x18;
        double* temp_4 = temp_3 - entity_15;
        double* context_3 = context_2 - entity_15;
        item_3c = item_3c + 2.0;
        arg3 = temp_4;
        double item_34_1 = item_34 + 2.0;
        int* source_9 = &arg2[-((result * 7)) * 4];
        char item_58_1 = arg6;
        double* item_54 = arg7;
        double item_2c_1 = fpu_val_7_1 + 16387.0;
        int* item_44_1 = source_9;
        double* i = nullptr;
        item_24 = item_24 - 2.0;
        double* item_64 = temp_4;
        double* item_60_1 = context_3;
        int* result_1 = result;
        int item_50_1 = arg9;
        int item_4c_1 = 0;
        int item_48;
        strncpy(&item_48, "DCBA", 4);
        double item_1c_1 = item_1c - 2.0;
        double item_14_1 = item_14 - 16387.0;
        int item_88;

        if (arg8 & 0x7fffffff)
        {
            if (!item_8)
            {
                do
                {
                    if (i == 1 || !i || i == 4 || i == 5)
                    {
                        int item_1b0_2 = arg8;
                        void** item_1c0_1 = &item_64;
                        DynObj_ParseConfigFile_9(context, 9, temp_4, context_3, result, i, item_88);
                        temp_4 = arg3;
                        result = arg5;
                    }

                    i += 1;
                } while (i < 6);
            }
            else
            {
                do
                {
                    if (i == 1 || !i || i == 4 || i == 5)
                        result = DynObj_CreateMagicObject_9(context, 0xa, &item_64, &item_24, &item_3c, arg8, i);

                    i += 1;
                } while (i < 6);
            }
        }

        if (arg8 & 0x80000000)
        {
            DynObj_ParseConfigFile_6();
            int source_10 = context;
            int context_4 = *(source_10 + 0xde4);
            int item_70_1 = *(source_10 + 0xc) + 0x20;
            int temp_8 = *(source_10 + 0xde8);
            int item_80_1 = 0;
            int item_7c_1 = 0;
            int item_78_1 = 0;
            int item_74_1 = 0;
            int data_13 = *(source_10 + 0xde0);
            int dest_4 = item_24 + data_13;
            int item_6c_1 = source_10;
            item_88 = 0x40000000;
            int item_84_1 = 0x1e;
            int data_18 = ((item_2c_1 + *(source_10 + 0xde8)) ^ (item_14_1 + temp_8))
                | ((item_34_1 + context_4) ^ (item_1c_1 + context_4)) | ((item_3c + data_13) ^ dest_4);
            arg6 = data_18;
            int eflags_1;
            int data_19;
            data_19 = __bsr_gprv_memv(arg6);
            arg6 = data_19;
            int data_20 = 0x1e - arg6;

            if (data_20 > 0x10)
                data_20 = 0x10;

            double* dest_5 = arg3;
            int* item_1b4_3 = &item_88;
            double* data_21 = DynObj_RegisterSymbol_10(source_10, &item_88, dest_5, data_20);
            result = arg5;

            if (result > 0)
            {
                arg6 = dest_5;
                double* data_23 = context_3 - dest_5;
                int* context_8 = &source_9[0x16];
                arg4 = data_23;
                arg2 = arg5;

                while (true)
                {
                    void item_104;

                    if (DynObj_InsertHashItem_11(&item_104, &item_88, data_21, dest_5, data_23 + dest_5))
                    {
                        double item_fc[0x6];
                        long double fp0_2;
                        long double f8_1;

                        if (*context_8)
                        {
                            double item_c_7 = Sys_PushStack_11(dest_5, &item_fc);
                            fp0_2 = Sys_PushStack_11(dest_5, &context_8[-0x14]);
                            f8_1 = item_c_7;
                            fp0_2 - f8_1;
                        }

                        if (!*context_8 || !(*(&((fp0_2 < f8_1 ? 1 : 0) << 8
                            | (FCMP_UO(fp0_2, f8_1) ? 1 : 0) << 0xa
                            | (fp0_2 == f8_1 ? 1 : 0) << 0xe) + 1) & 0x41))
                        {
                            int item_194_1 = 0;
                            int item_10c_1 = 0;
                            int item_108_1 = 0;
                            char item_190_1 = 1;
                            char item_110_1 = 0xff;
                            int item_198_1 = 1;
                            double (* item_19c)[0x6] = &item_fc;
                            DynObj_CreateMagicObject_11(source_10, 0xb, &item_19c, &item_fc, 0x20000000, 4);

                            if (item_190_1)
                            {
                                int item_a4;
                                context_8[2] = item_a4;
                                int item_a0;
                                context_8[3] = item_a0;
                                memcpy(&context_8[-0x14], &item_fc, 0x18);
                                int item_b8;
                                context_8[-3] = item_b8;
                                dest_5 = arg6;
                                source_10 = context;
                                int item_b4;
                                context_8[-2] = item_b4;
                                int item_cc;
                                context_8[-8] = item_cc;
                                *context_8 = 1;
                                context_8[-0x16] = 0;
                                int item_c8;
                                context_8[-7] = item_c8;
                                int item_c4;
                                context_8[-6] = item_c4;
                            }
                        }
                    }

                    context_8 = &context_8[0x1c];
                    dest_5 = &dest_5[3];
                    result = arg2 - 1;
                    bool flag_3_1 = arg2 != 1;
                    arg6 = dest_5;
                    arg2 = result;

                    if (!flag_3_1)
                        break;

                    data_23 = arg4;
                }
            }
        }
    }

    return result;
}
