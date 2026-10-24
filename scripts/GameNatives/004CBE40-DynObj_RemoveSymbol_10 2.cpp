// IGI native provenance: DynObj_RemoveSymbol_10 - 0x004CBE40 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CBE40-DynObj_RemoveSymbol_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_RemoveSymbol_10.h"
int* DynObj_RemoveSymbol_10(int context, int* arg2, double* arg3, int* arg4, double* arg5, char arg6, double* arg7, int arg8, int arg9)

{
    DynObj_ParseConfigFile_6();
    int item_4c = context;
    int item_68 = 0x40000000;
    int item_64 = 0x1e;
    int item_50 = *(context + 0xc) + 0x20;
    int item_60 = 0;
    int item_5c = 0;
    int item_58 = 0;
    int item_54 = 0;
    double item_350[0x2];

    if (arg4 > 0)
    {
        double* temp_1 = arg5;
        void item_340;
        void* data_3 = &item_340;
        void* context_1 = &arg2[0x16];
        void* entity_3 = arg3 - &item_350;
        int* item_8_1 = arg4;
        void* item_10_1 = entity_3;

        while (true)
        {
            long double fpu_val_7_1 = *temp_1;
            void* dest_1 = data_3 - 0x10;
            temp_1 = &temp_1[1];
            data_3 += 0x18;
            context_1 += 0x70;
            memcpy(dest_1, dest_1 + entity_3, 0x18);
            *(data_3 - 0x18) = fpu_val_7_1 + *(data_3 - 0x18);
            *(context_1 - 0x70) = 0;
            bool flag_1_1 = item_8_1 != 1;
            item_8_1 -= 1;

            if (!flag_1_1)
                break;

            entity_3 = item_10_1;
        }
    }

    double item_44;
    memcpy(&item_44, arg3, 0x18);
    double item_2c;
    memcpy(&item_2c, arg3, 0x18);
    int* i_1 = arg4;
    double item_3c;
    double item_34;
    double item_24;
    double item_1c;
    long double fpu_val_7_3;

    if (i_1 <= 0)
        fpu_val_7_3 = item_34;
    else
    {
        fpu_val_7_3 = item_34;
        double* entity_6 = &arg3[2];
        double* temp_2 = &item_350 - arg3;
        int* i;

        do
        {
            long double fpu_val_6_1 = item_2c;
            long double f2_1 = entity_6[-2];
            fpu_val_6_1 - f2_1;

            if (!(*(&((fpu_val_6_1 < f2_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_1, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_6_1 == f2_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double fpu_val_6_2 = item_44;
                long double f3_1 = entity_6[-2];
                fpu_val_6_2 - f3_1;

                if (!(*(&((fpu_val_6_2 < f3_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_2, f3_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_2 == f3_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                {
                    int dest_3 = *(entity_6 - 0xc);
                    item_44 = entity_6[-2];
                    *(&item_44 + 4) = dest_3;
                }
            }
            else
            {
                int dest_2 = *(entity_6 - 0xc);
                item_2c = entity_6[-2];
                *(&item_2c + 4) = dest_2;
            }

            long double fpu_val_6_3 = item_24;
            long double f4_1 = entity_6[-1];
            fpu_val_6_3 - f4_1;

            if (!(*(&((fpu_val_6_3 < f4_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_3, f4_1) ? 1 : 0) << 0xa
                | (fpu_val_6_3 == f4_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double fpu_val_6_4 = item_3c;
                long double f5_1 = entity_6[-1];
                fpu_val_6_4 - f5_1;

                if (!(*(&((fpu_val_6_4 < f5_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_4, f5_1) ? 1 : 0) << 0xa
                    | (fpu_val_6_4 == f5_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                {
                    item_3c = entity_6[-1];
                    *(&item_3c + 4) = *(entity_6 - 4);
                }
            }
            else
            {
                item_24 = entity_6[-1];
                *(&item_24 + 4) = *(entity_6 - 4);
            }

            long double fpu_val_6_5 = item_1c;
            long double f6_1 = *entity_6;
            fpu_val_6_5 - f6_1;

            if (!(*(&((fpu_val_6_5 < f6_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_5, f6_1) ? 1 : 0) << 0xa
                | (fpu_val_6_5 == f6_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double f7_1 = *entity_6;
                fpu_val_7_3 - f7_1;

                if (!(*(&((fpu_val_7_3 < f7_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_3, f7_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_3 == f7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                    fpu_val_7_3 = *entity_6;
            }
            else
            {
                item_1c = *entity_6;
                *(&item_1c + 4) = *(entity_6 + 4);
            }

            long double fpu_val_6_6 = item_1c;
            long double f9_1 = *(temp_2 + entity_6);
            fpu_val_6_6 - f9_1;

            if (!(*(&((fpu_val_6_6 < f9_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_6_6, f9_1) ? 1 : 0) << 0xa
                | (fpu_val_6_6 == f9_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1))
            {
                long double f10_1 = *(temp_2 + entity_6);
                fpu_val_7_3 - f10_1;

                if (!(*(&((fpu_val_7_3 < f10_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_3, f10_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_3 == f10_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                    fpu_val_7_3 = *(temp_2 + entity_6);
            }
            else
                item_1c = *(temp_2 + entity_6);

            entity_6 = &entity_6[3];
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
    }

    int data_11 = *(context + 0xde0);
    int context_2 = *(context + 0xde4);
    item_2c = item_2c + 2.0;
    int temp_3 = *(context + 0xde8);
    double item_24_1 = item_24 + 2.0;
    double item_1c_1 = item_1c + 16387.0;
    item_44 = item_44 - 2.0;
    *(&item_34 + 4) = item_3c - 2.0 + temp_3;
    int data_16 = ((item_1c_1 + temp_3) ^ *(&item_34 + 4))
        | ((item_24_1 + context_2) ^ (fpu_val_7_3 - 16387.0 + context_2))
        | ((item_2c + data_11) ^ (item_44 + data_11));
    int eflags;
    int data_17;
    data_17 = __bsr_gprv_memv(data_16);
    int data_18 = 0x1e - data_17;

    if (data_18 > 0x10)
        data_18 = 0x10;

    int* item_368 = &item_68;
    int data_19 = DynObj_RegisterSymbol_10(context, &item_68, arg3, data_18);
    int* result = arg8 & 0x7fffffff;

    if (result)
        result = World_TraceBatch(context, arg2, arg3, &item_350, arg4, arg6, arg7, result, arg9);

    if (arg8 & 0x80000000)
    {
        result = arg4;

        if (result > 0)
        {
            double* dest_10 = arg5;
            double* source_7 = &arg3[2];
            arg3 = dest_10;
            arg5 = source_7;
            double* context_6 = &arg2[6];
            arg4 = result;
            bool flag_2_1;

            do
            {
                int entity_16 = *dest_10;

                if (entity_16 < 0)
                    entity_16 = -(entity_16);

                long double fpu_val_7_11 = *dest_10;
                long double f8_1 = 0.0;
                fpu_val_7_11 - f8_1;
                int data_24;

                data_24 = *(&((fpu_val_7_11 < f8_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_11, f8_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_11 == f8_1 ? 1 : 0) << 0xe) + 1) & 0x41 ? 0 : 1;

                void item_d8;

                if (DynObj_CreateMagicObject_10(&item_d8, &item_68, &source_7[-2], data_24, entity_16, data_19, 1))
                {
                    double item_c0;
                    long double fpu_val_6_21;
                    long double fpu_val_7_13;

                    if (context_6[8])
                    {
                        fpu_val_7_13 = *source_7 - item_c0;
                        long double f13_1 = 0.0;
                        fpu_val_7_13 - f13_1;

                        if (*(&((fpu_val_7_13 < f13_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_13, f13_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_13 == f13_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                            fpu_val_7_13 = -(fpu_val_7_13);

                        fpu_val_6_21 = *source_7 - *context_6;
                        long double f15_1 = 0.0;
                        fpu_val_6_21 - f15_1;

                        if (*(&((fpu_val_6_21 < f15_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_6_21, f15_1) ? 1 : 0) << 0xa
                                | (fpu_val_6_21 == f15_1 ? 1 : 0) << 0xe | 0x3000) + 1) & 1)
                            fpu_val_6_21 = -(fpu_val_6_21);

                        fpu_val_7_13 - fpu_val_6_21;
                    }

                    if (!context_6[8] || *(&((fpu_val_7_13 < fpu_val_6_21 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_13, fpu_val_6_21) ? 1 : 0) << 0xa
                        | (fpu_val_7_13 == fpu_val_6_21 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    {
                        int item_168_1 = 0;
                        int item_e0_1 = 0;
                        int item_dc_1 = 0;
                        double item_d0[0x2];
                        double (* item_170)[0x2] = &item_d0;
                        char item_164_1 = 1;
                        char item_e4_1 = 0xff;
                        int item_16c_1 = 1;
                        DynObj_CreateMagicObject_11(context, 0xb, &item_170, &item_d0, 0x20000000, 4);
                        short data_31;
                        data_31 = item_164_1;

                        if (data_31)
                        {
                            long double fpu_val_7_17 = item_c0 - *source_7;
                            long double f14_1 = 0.0;
                            fpu_val_7_17 - f14_1;
                            data_31 = (fpu_val_7_17 < f14_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_17, f14_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_17 == f14_1 ? 1 : 0) << 0xe | 0x3800;

                            if (*(&data_31 + 1) & 1)
                                fpu_val_7_17 = -(fpu_val_7_17);

                            memcpy(&context_6[-2], &item_d0, 0x18);
                            context_6[9] = fpu_val_7_17;
                            int item_8c;
                            *(context_6 + 0x34) = item_8c;
                            source_7 = arg5;
                            dest_10 = arg3;
                            int item_88;
                            context_6[7] = item_88;
                            int item_a0;
                            context_6[4] = item_a0;
                            context_6[8] = 1;
                            context_6[-3] = 0;
                            int item_9c;
                            *(context_6 + 0x24) = item_9c;
                            int item_98;
                            context_6[5] = item_98;
                        }
                    }
                }

                context_6 = &context_6[0xe];
                dest_10 = &dest_10[1];
                source_7 = &source_7[3];
                result = arg4 - 1;
                flag_2_1 = arg4 != 1;
                arg3 = dest_10;
                arg5 = source_7;
                arg4 = result;
            } while (flag_2_1);
        }
    }

    return result;
}
