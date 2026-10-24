// IGI native provenance: Weapon_UpdateZoom_8 - 0x0047D920 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047D920-Weapon_UpdateZoom_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Weapon_UpdateZoom_8.h"
int Weapon_UpdateZoom_8(double* context, float* arg2, void* arg3, int* arg4, void* arg5, int arg6, int arg7, float arg8)

{
    arg4[0x12] = arg6;
    arg4[0x13] = arg7;
    arg4[0x15] = 0;
    arg4[0x14] = arg8;
    arg4[3] = *(arg5 + 0x20);
    arg4[4] = *(arg5 + 0x24);
    arg4[5] = *(arg5 + 0x28);
    *arg4 = *(arg5 + 0x2c);
    arg4[1] = *(arg5 + 0x30);
    arg4[2] = *(arg5 + 0x34);
    long double fpu7 = *arg4;
    long double f0 = 0.0f;
    fpu7 - f0;

    if (!(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40))
    {
    label_47d9b2:
        long double fpu_val_7_3 = arg4[2];
        long double fpu_val_6_1 = arg4[1];
        long double fpu_val_5_1 = *arg4;
        long double fpu_val_7_5 =
            1.0 / sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_3 * fpu_val_7_3);
        *arg4 = *arg4 * fpu_val_7_5;
        arg4[1] = arg4[1] * fpu_val_7_5;
        arg4[2] = arg4[2] * fpu_val_7_5;
    }
    else
    {
        long double fpu_val_7_1 = arg4[1];
        long double f1_1 = 0.0f;
        fpu_val_7_1 - f1_1;

        if (!(*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_47d9b2;

        long double fpu_val_7_2 = arg4[2];
        long double f2_1 = 0.0f;
        fpu_val_7_2 - f2_1;

        if (!(*(&((fpu_val_7_2 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_2 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_47d9b2;
    }

    arg4[6] = arg8;
    arg4[8] = 0x41449ba6;
    arg4[0xb] = 0x41449ba6;
    arg4[0xe] = 0x41449ba6;
    arg4[0x11] = 0x41449ba6;
    long double fpu_val_7_8 = -(arg8);
    arg4[9] = fpu_val_7_8;
    arg4[0xd] = fpu_val_7_8;
    arg4[0xf] = fpu_val_7_8;
    arg4[0x10] = fpu_val_7_8;
    arg4[7] = arg8;
    arg4[0xa] = arg8;
    arg4[0xc] = arg8;
    float item_d0;
    Sys_ResolveSymbol_12(&item_d0, arg5 + 0x2c);
    float item_cc;
    float item_c0;
    float item_b4;
    float item_f0 = item_c0 * arg4[7] + item_b4 * arg4[8] + item_cc * arg4[6];
    float item_bc;
    float item_b0;
    long double fpu_val_7_19 = item_bc * arg4[7] + item_b0 * arg4[8];
    float item_c8;
    long double fpu_val_6_20 = item_c8 * arg4[6];
    float item_c4;
    float item_b8;
    arg4[6] = item_c4 * arg4[7] + item_b8 * arg4[8] + item_d0 * arg4[6];
    arg4[7] = item_f0;
    long double fpu_val_7_22 = item_c4 * arg4[0xa];
    long double fpu_val_6_22 = item_b8 * arg4[0xb];
    arg4[8] = fpu_val_7_19 + fpu_val_6_20;
    float item_f0_1 = item_c0 * arg4[0xa] + item_b4 * arg4[0xb] + item_cc * arg4[9];
    long double fpu_val_7_32 = item_bc * arg4[0xa] + item_b0 * arg4[0xb] + item_c8 * arg4[9];
    arg4[9] = fpu_val_7_22 + fpu_val_6_22 + item_d0 * arg4[9];
    arg4[0xa] = item_f0_1;
    long double fpu_val_7_34 = item_b8 * arg4[0xe];
    long double fpu_val_6_34 = item_d0 * arg4[0xc];
    arg4[0xb] = fpu_val_7_32;
    float item_f0_2 = item_b4 * arg4[0xe] + item_cc * arg4[0xc] + item_c0 * arg4[0xd];
    long double fpu_val_7_43 = item_b0 * arg4[0xe] + item_c8 * arg4[0xc];
    long double fpu_val_6_44 = item_bc * arg4[0xd];
    arg4[0xc] = fpu_val_7_34 + fpu_val_6_34 + item_c4 * arg4[0xd];
    arg4[0xd] = item_f0_2;
    long double fpu_val_7_46 = item_c4 * arg4[0x10];
    long double fpu_val_6_46 = item_b8 * arg4[0x11];
    arg4[0xe] = fpu_val_7_43 + fpu_val_6_44;
    float item_f0_3 = item_c0 * arg4[0x10] + item_b4 * arg4[0x11] + item_cc * arg4[0xf];
    long double fpu_val_7_55 = item_bc * arg4[0x10] + item_b0 * arg4[0x11];
    long double fpu_val_6_56 = item_c8 * arg4[0xf];
    arg4[0xf] = fpu_val_7_46 + fpu_val_6_46 + item_d0 * arg4[0xf];
    arg4[0x10] = item_f0_3;
    long double fpu_val_7_57 = arg4[6];
    arg4[0x11] = fpu_val_7_55 + fpu_val_6_56;
    arg4[6] = fpu_val_7_57 + *(arg5 + 0x20);
    arg4[7] = arg4[7] + *(arg5 + 0x24);
    arg4[8] = arg4[8] + *(arg5 + 0x28);
    arg4[9] = arg4[9] + *(arg5 + 0x20);
    arg4[0xa] = arg4[0xa] + *(arg5 + 0x24);
    arg4[0xb] = *(arg5 + 0x28) + arg4[0xb];
    arg4[0xc] = arg4[0xc] + *(arg5 + 0x20);
    arg4[0xd] = *(arg5 + 0x24) + arg4[0xd];
    arg4[0xe] = arg4[0xe] + *(arg5 + 0x28);
    arg4[0xf] = arg4[0xf] + *(arg5 + 0x20);
    arg4[0x10] = arg4[0x10] + *(arg5 + 0x24);
    arg4[0x11] = *(arg5 + 0x28) + arg4[0x11];
    *(arg3 + 0xe138) = 1;
    float entity_10 = *(arg5 + 0x38);
    float temp_13 = *(arg5 + 0x3c);
    float data_9 = *(arg5 + 0x40);
    long double fpu_val_7_81 = data_9;
    long double fpu_val_6_57 = temp_13;
    long double fpu_val_5_3 = entity_10;
    long double fpu_val_7_83 =
        24.5760002f / sqrtl(fpu_val_5_3 * fpu_val_5_3 + fpu_val_6_57 * fpu_val_6_57 + fpu_val_7_81 * fpu_val_7_81);
    float item_f4_5 = entity_10 * fpu_val_7_83;
    float item_f0_5 = temp_13 * fpu_val_7_83;
    long double fpu_val_7_84 = fpu_val_7_83 * data_9;
    long double fpu_val_6_63 = arg4[6];
    long double fpu_val_5_5 = arg4[7];
    double item_70 = fpu_val_5_5;
    long double fpu_val_4_11 = arg4[8];
    double item_68 = fpu_val_4_11;
    double item_e8 = arg2[1] * fpu_val_5_5 + arg2[2] * fpu_val_4_11 + *arg2 * fpu_val_6_63;
    double item_e0 = arg2[3] * fpu_val_6_63 + arg2[4] * fpu_val_5_5 + arg2[5] * fpu_val_4_11;
    double item_d8 = arg2[7] * fpu_val_5_5 + arg2[8] * fpu_val_4_11 + arg2[6] * fpu_val_6_63;
    double item_78;
    memcpy(&item_78, &item_e8, 0x18);
    long double fpu_val_6_66 = item_e8 + *context;
    item_78 = fpu_val_6_66;
    double item_70_1 = item_70 + context[1];
    double item_68_1 = item_68 + context[2];
    long double fpu_val_5_11 = arg4[9];
    long double fpu_val_4_13 = arg4[0xa];
    double item_88 = fpu_val_4_13;
    long double fpu_val_3_21 = arg4[0xb];
    double item_80 = fpu_val_3_21;
    item_e8 = arg2[1] * fpu_val_4_13 + arg2[2] * fpu_val_3_21 + *arg2 * fpu_val_5_11;
    double item_e0_1 = arg2[4] * fpu_val_4_13 + arg2[5] * fpu_val_3_21 + arg2[3] * fpu_val_5_11;
    double item_d8_1 = arg2[7] * fpu_val_4_13 + arg2[8] * fpu_val_3_21 + arg2[6] * fpu_val_5_11;
    long double fpu_val_5_14 = item_e8 + *context;
    double item_90;
    memcpy(&item_90, &item_e8, 0x18);
    item_90 = fpu_val_5_14;
    double item_88_1 = item_88 + context[1];
    double item_80_1 = item_80 + context[2];
    long double fpu_val_5_19 = arg4[0xc];
    long double fpu_val_4_15 = arg4[0xd];
    double item_a0 = fpu_val_4_15;
    long double fpu_val_3_23 = arg4[0xe];
    double item_98 = fpu_val_3_23;
    item_e8 = arg2[1] * fpu_val_4_15 + arg2[2] * fpu_val_3_23 + *arg2 * fpu_val_5_19;
    double item_e0_2 = arg2[4] * fpu_val_4_15 + arg2[5] * fpu_val_3_23 + arg2[3] * fpu_val_5_19;
    double item_d8_2 = arg2[7] * fpu_val_4_15 + arg2[8] * fpu_val_3_23 + arg2[6] * fpu_val_5_19;
    double item_a8;
    memcpy(&item_a8, &item_e8, 0x18);
    item_a8 = item_e8 + *context;
    double item_a0_1 = item_a0 + context[1];
    double item_98_1 = item_98 + context[2];
    long double fpu_val_5_27 = arg4[0xf];
    long double fpu_val_4_17 = arg4[0x10];
    double item_e0_3 = fpu_val_4_17;
    long double fpu_val_3_25 = arg4[0x11];
    double item_d8_3 = fpu_val_3_25;
    double item_60 = arg2[1] * fpu_val_4_17 + arg2[2] * fpu_val_3_25 + *arg2 * fpu_val_5_27;
    double item_58 = arg2[4] * fpu_val_4_17 + arg2[5] * fpu_val_3_25 + arg2[3] * fpu_val_5_27;
    float data_11 = g_a44344;
    double item_50 = arg2[8] * fpu_val_3_25 + arg2[6] * fpu_val_5_27 + arg2[7] * fpu_val_4_17;
    memcpy(&item_e8, &item_60, 0x18);
    item_e8 = item_60 + *context;
    double item_e0_4 = item_e0_3 + context[1];
    double item_d8_4 = item_d8_3 + context[2];
    item_60 = fpu_val_6_66 - item_f4_5;
    double item_58_1 = item_70_1 - item_f0_5;
    double item_50_1 = item_68_1 - fpu_val_7_84;
    double item_18 = item_90 - item_f4_5;
    double item_10 = item_88_1 - item_f0_5;
    double item_8 = item_80_1 - fpu_val_7_84;
    double item_48 = item_a8 - item_f4_5;
    double item_40 = item_a0_1 - item_f0_5;
    double item_38 = item_98_1 - fpu_val_7_84;
    double item_30 = item_e8 - item_f4_5;
    double item_28 = item_e0_4 - item_f0_5;
    double item_20 = item_d8_4 - fpu_val_7_84;
    int data_12 =
        DynObj_RegisterSymbol_12(data_11, &item_78, &item_60, 0xff, 0.0f, 0xffffffff, 0xffffffff, nullptr);
    char f3 = data_12;
    data_12 = -(data_12);
    int data_16 =
        DynObj_RegisterSymbol_12(g_a44344, &item_90, &item_18, 0xff, 0.0f, 0xffffffff, 0xffffffff, nullptr);
    char f4 = data_16;
    data_16 = -(data_16);
    int data_20 =
        DynObj_RegisterSymbol_12(g_a44344, &item_a8, &item_48, 0xff, 0.0f, 0xffffffff, 0xffffffff, nullptr);
    char f5 = data_20;
    data_20 = -(data_20);
    int data_24 =
        DynObj_RegisterSymbol_12(g_a44344, &item_e8, &item_30, 0xff, 0.0f, 0xffffffff, 0xffffffff, nullptr);
    char f6 = data_24;
    data_24 = -(data_24);

    if (!((data_12 - data_12 + 1) & (data_16 - data_16 + 1) & (data_20 - data_20 + 1)
            & (data_24 - data_24 + 1)))
        arg4[0x12] = 0xffffffff;

    return data_24 - data_24 + 1;
}
