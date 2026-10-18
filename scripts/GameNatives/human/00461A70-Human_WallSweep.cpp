// IGI native provenance: Human_WallSweep - 0x00461A70 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 00461A70-Human_WallSweep.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Human_WallSweep.hpp"
int Human_WallSweep(void* context, double* arg2, int* arg3, float* arg4, int* arg5)

{
    float item_25c;
    float* data_1 = FX_ExplosionSpawn_16(&item_25c, context, 0);
    float item_280 = *data_1;
    int item_28c = 0;
    int item_27c = data_1[1];
    float temp_1 = data_1[2];
    *arg5 &= 0xfffffff7;
    double* context_1;
    double* source_4;

    while (true)
    {
        source_4 = arg3;
        context_1 = arg2;
        char item_299_1 = 0;
        double item_250 = *source_4 - *context_1;
        double item_248 = source_4[1] - context_1[1];
        double item_240_1 = source_4[2] - context_1[2];

        if (Sys_PopStack_11(&item_250))
            break;

        double item_288_1 = Sys_OpenFileStream_11(&item_250) + 10.24;
        Sys_FreeMemory_11(&item_250);
        long double fpu_val_7_10 = item_288_1 * item_250;
        void item_22c;
        memcpy(&item_22c, &item_250, 0x18);
        double item_224 = item_224 * item_288_1;
        double item_21c = item_21c * item_288_1;
        item_28c += 1;
        double item_204_1 = 409.60000000000002 - temp_1;
        char data_5 = *(context + 0x33d);
        int item_214 = 0;
        double item_1a0 = fpu_val_7_10;
        long double fpu_val_6_7 = item_224;
        double item_238_1 = fpu_val_6_7;
        double item_198_1 = fpu_val_6_7;
        int item_210_1 = 0;
        int item_20c_1 = 0;
        int item_208_1 = 0;
        double item_190_1 = item_204_1 + item_21c;
        long double fpu_val_5_3;

        fpu_val_5_3 = !data_5 ? 1638.4000000000001 : 819.20000000000005;

        int item_1fc_1 = 0;
        int item_1f8_1 = 0;
        int item_1f4_1 = 0;
        int item_1f0_1 = 0;
        double item_188_1 = fpu_val_7_10;
        double item_180_1 = fpu_val_6_7;
        double item_178_1 = fpu_val_5_3 - temp_1 + item_21c;
        long double fpu_val_6_10;

        fpu_val_6_10 = !data_5 ? 7372.8000000000002 : 5324.8000000000002;

        int item_168 = item_238_1;
        int item_1e4_1 = 0;
        int item_1e0_1 = 0;
        int item_1dc_1 = 0;
        double item_170_1 = fpu_val_7_10;
        long double fpu_val_7_12 = fpu_val_6_10 - temp_1 + item_21c;
        int item_1d8 = 0;
        int item_164_1 = *(&item_238_1 + 4);
        int i = 0;
        double item_160_1 = fpu_val_7_12;
        double item_1b8;
        double item_1b0;
        void item_1a8;

        do
        {
            long double fpu_val_7_14 = *(&item_214 + i) + *context_1;
            i += 0x18;
            *(&item_22c + i) = fpu_val_7_14;
            *(&item_224 + i) = *(&item_224 + i) + context_1[1];
            *(&item_21c + i) = context_1[2] + *(&item_21c + i);
            *(&item_1b8 + i) = *(&item_1b8 + i) + *context_1;
            *(&item_1b0 + i) = *(&item_1b0 + i) + context_1[1];
            *(&item_1a8 + i) = context_1[2] + *(&item_1a8 + i);
        } while (i < 0x48);

        void* item_1a4_1 = context;
        void item_158;
        memset(&item_158, 0, 0x150);
        char data_7 = Level_LoadObjects_4();
        World_TraceBatch(Level_StartMission_4(), &item_158, &item_214, &item_1a0, 3, data_7, &item_1a8,
            0xffffffff, 1);
        void* context_2 = nullptr;
        double item_288_2 = item_288_1 - 10.24;
        int* temp_6;

        if (*arg5 & 4)
        {
            void item_b0;
            float* entity_10 = &item_b0;
            int i_4 = 2;
            int i_1;

            do
            {
                if (entity_10[8])
                {
                    int* data_22 = entity_10;
                    float source_1 = *data_22;
                    long double fpu_val_7_61 = source_1;
                    float source_2 = data_22[1];
                    long double f2_1 = 0.0f;
                    fpu_val_7_61 - f2_1;
                    long double fpu_val_7_62 = data_22[2];

                    if (!(*(&((fpu_val_7_61 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_61, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_61 == f2_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                    {
                    label_462120:
                        long double fpu_val_5_17 = source_2;
                        long double fpu_val_4_15 = source_1;
                        fpu_val_7_62 = 1.0 / sqrtl(fpu_val_4_15 * fpu_val_4_15 + fpu_val_5_17 * fpu_val_5_17
                            + fpu_val_7_62 * fpu_val_7_62) * fpu_val_7_62;
                    }
                    else
                    {
                        long double fpu_val_6_32 = source_2;
                        long double f5_1 = 0.0f;
                        fpu_val_6_32 - f5_1;

                        if (!(*(&((fpu_val_6_32 < f5_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_6_32, f5_1) ? 1 : 0) << 0xa
                                | (fpu_val_6_32 == f5_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                            goto label_462120;

                        long double f8_1 = 0.0f;
                        fpu_val_7_62 - f8_1;

                        if (!(*(&((fpu_val_7_62 < f8_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_62, f8_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_62 == f8_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                            goto label_462120;
                    }

                    long double f7_1 = 0.0f;
                    fpu_val_7_62 - f7_1;

                    if (*(&((fpu_val_7_62 < f7_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_62, f7_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_62 == f7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                        fpu_val_7_62 = -(fpu_val_7_62);

                    long double f11_1 = 0.100000001f;
                    fpu_val_7_62 - f11_1;

                    if (*(&((fpu_val_7_62 < f11_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_62, f11_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_62 == f11_1 ? 1 : 0) << 0xe) + 1) & 1)
                    {
                        entity_10[2] = 0;
                        long double fpu_val_7_64 = *entity_10;
                        long double f14_1 = 0.0f;
                        fpu_val_7_64 - f14_1;
                        long double fpu_val_7_65;
                        long double f18_1;

                        if (*(&((fpu_val_7_64 < f14_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_64, f14_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_64 == f14_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                        {
                            fpu_val_7_65 = entity_10[1];
                            f18_1 = 0.0f;
                            fpu_val_7_65 - f18_1;
                        }

                        if (!(*(&((fpu_val_7_64 < f14_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_64, f14_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_64 == f14_1 ? 1 : 0) << 0xe) + 1) & 0x40) || !(*(&(
                            (fpu_val_7_65 < f18_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_65, f18_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_65 == f18_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                        {
                            long double fpu_val_7_66 = entity_10[1];
                            long double fpu_val_6_37 = *entity_10;
                            long double fpu_val_7_71 =
                                1.0 / sqrtl(fpu_val_7_66 * fpu_val_7_66 + fpu_val_6_37 * fpu_val_6_37);
                            *entity_10 = *entity_10 * fpu_val_7_71;
                            entity_10[1] = entity_10[1] * fpu_val_7_71;
                            entity_10[2] = entity_10[2] * fpu_val_7_71;
                        }

                        long double fpu_val_7_73;
                        long double f24_1;

                        if (context_2)
                        {
                            fpu_val_7_73 = *(entity_10 + 0x28);
                            f24_1 = *(context_2 + 0x60);
                            fpu_val_7_73 - f24_1;
                        }

                        if (!context_2 || *(&((fpu_val_7_73 < f24_1 ? 1 : 0) << 8
                                | (FCMP_UO(fpu_val_7_73, f24_1) ? 1 : 0) << 0xa
                                | (fpu_val_7_73 == f24_1 ? 1 : 0) << 0xe) + 1) & 1)
                            context_2 = &entity_10[-0xe];
                    }
                }

                entity_10 = &entity_10[0x1c];
                i_1 = i_4;
                i_4 -= 1;
            } while (i_1 != 1);
            temp_6 = arg3;
            goto label_4621e4;
        }

        float item_120;
        int* entity_4 = &item_120;
        int i_3 = 3;
        int i_2;

        do
        {
            if (entity_4[8])
            {
                float dest_1 = *entity_4;
                long double fpu_val_7_27 = dest_1;
                float dest_2 = entity_4[1];
                long double f4_1 = 0.0f;
                fpu_val_7_27 - f4_1;
                long double fpu_val_7_28 = entity_4[2];

                if (!(*(&((fpu_val_7_27 < f4_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_27, f4_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_27 == f4_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                {
                label_461dfb:
                    long double fpu_val_5_8 = dest_2;
                    long double fpu_val_4_3 = dest_1;
                    fpu_val_7_28 = 1.0
                        / sqrtl(fpu_val_4_3 * fpu_val_4_3 + fpu_val_5_8 * fpu_val_5_8 + fpu_val_7_28 * fpu_val_7_28)
                        * fpu_val_7_28;
                }
                else
                {
                    long double fpu_val_6_12 = dest_2;
                    long double f6_1 = 0.0f;
                    fpu_val_6_12 - f6_1;

                    if (!(*(&((fpu_val_6_12 < f6_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_6_12, f6_1) ? 1 : 0) << 0xa
                            | (fpu_val_6_12 == f6_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                        goto label_461dfb;

                    long double f10_1 = 0.0f;
                    fpu_val_7_28 - f10_1;

                    if (!(*(&((fpu_val_7_28 < f10_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_28, f10_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_28 == f10_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x40))
                        goto label_461dfb;
                }

                long double f9_1 = 0.0f;
                fpu_val_7_28 - f9_1;

                if (*(&((fpu_val_7_28 < f9_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_28, f9_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_28 == f9_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 1)
                    fpu_val_7_28 = -(fpu_val_7_28);

                long double f13_1 = 0.100000001f;
                fpu_val_7_28 - f13_1;

                if (*(&((fpu_val_7_28 < f13_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_28, f13_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_28 == f13_1 ? 1 : 0) << 0xe) + 1) & 1)
                {
                    entity_4[2] = 0;
                    long double fpu_val_7_30 = *entity_4;
                    long double f16_1 = 0.0f;
                    fpu_val_7_30 - f16_1;
                    long double fpu_val_7_31;
                    long double f22_1;

                    if (*(&((fpu_val_7_30 < f16_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_30, f16_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_30 == f16_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                    {
                        fpu_val_7_31 = entity_4[1];
                        f22_1 = 0.0f;
                        fpu_val_7_31 - f22_1;
                    }

                    if (!(*(&((fpu_val_7_30 < f16_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_30, f16_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_30 == f16_1 ? 1 : 0) << 0xe) + 1) & 0x40) || !(*(&(
                        (fpu_val_7_31 < f22_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_31, f22_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_31 == f22_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    {
                        long double fpu_val_7_32 = entity_4[1];
                        long double fpu_val_6_17 = *entity_4;
                        long double fpu_val_7_37 =
                            1.0 / sqrtl(fpu_val_7_32 * fpu_val_7_32 + fpu_val_6_17 * fpu_val_6_17);
                        *entity_4 = *entity_4 * fpu_val_7_37;
                        entity_4[1] = entity_4[1] * fpu_val_7_37;
                        entity_4[2] = entity_4[2] * fpu_val_7_37;
                    }

                    long double fpu_val_7_39;
                    long double f27_1;

                    if (context_2)
                    {
                        fpu_val_7_39 = *(entity_4 + 0x28);
                        f27_1 = *(context_2 + 0x60);
                        fpu_val_7_39 - f27_1;
                    }

                    if (!context_2 || *(&((fpu_val_7_39 < f27_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_39, f27_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_39 == f27_1 ? 1 : 0) << 0xe) + 1) & 1)
                        context_2 = &entity_4[-0xe];
                }
            }

            entity_4 = &entity_4[0x1c];
            i_2 = i_3;
            i_3 -= 1;
        } while (i_2 != 1);
        int item_100;

        if (!item_100 || context_2)
            temp_6 = arg3;
        else
        {
            long double fpu_val_7_40 = item_120;
            long double f12_1 = 0.0f;
            fpu_val_7_40 - f12_1;
            float item_298_2 = item_120;
            float item_11c;
            float item_294_2 = item_11c;
            long double fpu_val_7_46;
            float item_118;

            if (!(*(&((fpu_val_7_40 < f12_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_40, f12_1) ? 1 : 0) << 0xa
                | (fpu_val_7_40 == f12_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            {
            label_461f28:
                long double fpu_val_7_43 = item_118;
                long double fpu_val_6_28 = item_11c;
                long double fpu_val_5_13 = item_120;
                fpu_val_7_46 = 1.0
                    / sqrtl(fpu_val_5_13 * fpu_val_5_13 + fpu_val_6_28 * fpu_val_6_28 + fpu_val_7_43 * fpu_val_7_43)
                    * item_118;
            }
            else
            {
                long double fpu_val_7_41 = item_11c;
                long double f15_1 = 0.0f;
                fpu_val_7_41 - f15_1;

                if (!(*(&((fpu_val_7_41 < f15_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_41, f15_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_41 == f15_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_461f28;

                long double fpu_val_7_42 = item_118;
                long double f21_1 = 0.0f;
                fpu_val_7_42 - f21_1;

                if (!(*(&((fpu_val_7_42 < f21_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_42, f21_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_42 == f21_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_461f28;

                fpu_val_7_46 = item_118;
            }
            long double f20_1 = 0.999899983f;
            fpu_val_7_46 - f20_1;

            if (*(&((fpu_val_7_46 < f20_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_46, f20_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_46 == f20_1 ? 1 : 0) << 0xe) + 1) & 0x41)
                temp_6 = arg3;
            else
            {
                temp_6 = arg3;
                void item_150;
                memcpy(temp_6, &item_150, 0x18);
                *(temp_6 + 0x10) = temp_1 - 409.60000000000002 + *(temp_6 + 0x10);
            }
        }

        int item_20;

        if (!item_20)
        {
        label_4621e4:

            if (context_2)
            {
            label_4621f3:
                double item_274 = *(context_2 + 0x38);
                double item_26c = *(context_2 + 0x3c);
                double item_264_1 = *(context_2 + 0x40);
                Sys_FreeMemory_11(&item_274);
                long double fpu_val_7_78 = item_288_2 * item_250;
                void item_1c0;
                memcpy(&item_1c0, &item_250, 0x18);
                item_1b8 = item_1b8 * item_288_2;
                long double fpu_val_6_51 = item_1b0 * item_288_2;
                item_1b0 = fpu_val_6_51;
                long double fpu_val_7_80 =
                    fpu_val_6_51 * item_264_1 + item_1b8 * item_26c + fpu_val_7_78 * item_274;
                long double f17_1 = 0.0;
                fpu_val_7_80 - f17_1;

                if (*(&((fpu_val_7_80 < f17_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_80, f17_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_80 == f17_1 ? 1 : 0) << 0xe) + 1) & 1)
                {
                    float item_1cc = *arg3 - *(context_2 + 8);
                    float item_1c8_1 = *(arg3 + 8) - *(context_2 + 0x10);
                    float item_1c4_1 = *(arg3 + 0x10) - *(context_2 + 0x18);
                    Sys_PopStack_10(&item_25c, &item_1cc, context_2 + 0x38, context_2 + 0x38);
                    item_299_1 = 1;
                    *arg3 = item_25c + *(context_2 + 8);
                    float item_258;
                    *(arg3 + 8) = item_258 + *(context_2 + 0x10);
                    float item_254;
                    *(arg3 + 0x10) = item_254 + *(context_2 + 0x18);
                    long double fpu_val_7_94 = item_274 * 10.24;
                    item_26c = item_26c * 10.24;
                    double item_264_2 = item_264_1 * 10.24;
                    *arg3 = fpu_val_7_94 + *arg3;
                    *(arg3 + 8) = item_26c + *(arg3 + 8);
                    *(arg3 + 0x10) = item_264_2 + *(arg3 + 0x10);
                    *arg5 |= 8;
                }

                temp_6 = arg3;
            }
        }
        else
        {
            if (context_2)
                goto label_4621f3;

            float item_40;
            long double fpu_val_7_50 = item_40;
            long double f19_1 = 0.0f;
            fpu_val_7_50 - f19_1;
            float item_3c;
            float item_294_3 = item_3c;
            float item_298_3 = item_40;
            long double fpu_val_7_56;
            float item_38;

            if (!(*(&((fpu_val_7_50 < f19_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_50, f19_1) ? 1 : 0) << 0xa
                | (fpu_val_7_50 == f19_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            {
            label_462011:
                long double fpu_val_7_53 = item_38;
                long double fpu_val_6_30 = item_3c;
                long double fpu_val_5_15 = item_40;
                fpu_val_7_56 = 1.0
                    / sqrtl(fpu_val_5_15 * fpu_val_5_15 + fpu_val_6_30 * fpu_val_6_30 + fpu_val_7_53 * fpu_val_7_53)
                    * item_38;
            }
            else
            {
                long double fpu_val_7_51 = item_3c;
                long double f23_1 = 0.0f;
                fpu_val_7_51 - f23_1;

                if (!(*(&((fpu_val_7_51 < f23_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_51, f23_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_51 == f23_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_462011;

                long double fpu_val_7_52 = item_38;
                long double f26_1 = 0.0f;
                fpu_val_7_52 - f26_1;

                if (!(*(&((fpu_val_7_52 < f26_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_52, f26_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_52 == f26_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                    goto label_462011;

                fpu_val_7_56 = item_38;
            }
            long double f25_1 = -0.999899983f;
            fpu_val_7_56 - f25_1;
            void* data_18;

            if (!(*(&((fpu_val_7_56 < f25_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_56, f25_1) ? 1 : 0) << 0xa
                | (fpu_val_7_56 == f25_1 ? 1 : 0) << 0xe) + 1) & 1))
            {
                int item_70;
                *temp_6 = item_70;
                int item_6c;
                temp_6[1] = item_6c;
                int item_68;
                temp_6[2] = item_68;
                int item_64;
                temp_6[3] = item_64;
                data_18 = context;
                *(data_18 + 0x664) = 0;
                *(data_18 + 0x668) = 0;
            }
            else
                data_18 = context;

            *(data_18 + 0x66c) = 0;
            double item_60;
            *(temp_6 + 0x10) = item_60 - (7372.8000000000002 - temp_1) - 10.24;
        }

        if (item_28c > 0xa)
        {
            *temp_6 = *arg2;
            temp_6[1] = *(arg2 + 4);
            temp_6[2] = arg2[1];
            temp_6[3] = *(arg2 + 0xc);
        }
        else if (item_299_1)
            continue;

        context_1 = arg2;
        source_4 = arg3;
        break;
    }

    *arg4 = *source_4 - *context_1;
    arg4[1] = source_4[1] - context_1[1];
    int item_2ac_5 = item_28c;
    arg4[2] = source_4[2] - context_1[2];
    return AI_CheckHearing_18(context, "Checks: %d\n");
}
