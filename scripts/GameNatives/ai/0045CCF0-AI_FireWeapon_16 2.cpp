// IGI native provenance: AI_FireWeapon_16 - 0x0045CCF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045CCF0-AI_FireWeapon_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_16.h"
int* AI_FireWeapon_16(void* context, int* arg2)

{
    int dest_1 = *(context + 8) & 0xf003e01c;
    int* result = *(arg2[0x1c] + 4);

    if (*(&result + 1) & 1)
        dest_1 |= 1;

    if (result & 0x10000)
        dest_1 |= 2;

    result = arg2[0x340];
    int dest_2;

    dest_2 = !result ? dest_1 & 0xfffbffff : dest_1 | 0x40000;

    long double fpu7 = arg2[0x6e];
    long double f0 = 0.0f;
    fpu7 - f0;
    result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x20;

    long double fpu_val_7_1 = arg2[0x6f];
    long double f1 = 0.0f;
    fpu_val_7_1 - f1;
    result = (fpu_val_7_1 < f1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1) ? 1 : 0) << 0xa
        | (fpu_val_7_1 == f1 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x40;

    long double fpu_val_7_2 = arg2[0x70];
    long double f2 = 0.0f;
    fpu_val_7_2 - f2;
    result = (fpu_val_7_2 < f2 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2) ? 1 : 0) << 0xa
        | (fpu_val_7_2 == f2 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x80;

    long double fpu_val_7_3 = arg2[0x71];
    long double f3 = 0.0f;
    fpu_val_7_3 - f3;
    result = (fpu_val_7_3 < f3 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_3, f3) ? 1 : 0) << 0xa
        | (fpu_val_7_3 == f3 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x100;

    long double fpu_val_7_4 = arg2[0x72];
    long double f4 = 0.0f;
    fpu_val_7_4 - f4;
    result = (fpu_val_7_4 < f4 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_4, f4) ? 1 : 0) << 0xa
        | (fpu_val_7_4 == f4 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x200;

    long double fpu_val_7_5 = arg2[0x73];
    long double f5 = 0.0f;
    fpu_val_7_5 - f5;
    result = (fpu_val_7_5 < f5 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_5, f5) ? 1 : 0) << 0xa
        | (fpu_val_7_5 == f5 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x400;

    long double fpu_val_7_6 = arg2[0x74];
    long double f6 = 0.0f;
    fpu_val_7_6 - f6;
    result = (fpu_val_7_6 < f6 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_6, f6) ? 1 : 0) << 0xa
        | (fpu_val_7_6 == f6 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x800;

    long double fpu_val_7_7 = arg2[0x75];
    long double f7 = 0.0f;
    fpu_val_7_7 - f7;
    result = (fpu_val_7_7 < f7 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_7, f7) ? 1 : 0) << 0xa
        | (fpu_val_7_7 == f7 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x1000;

    long double fpu_val_7_8 = arg2[0x81];
    long double f8 = 0.0f;
    fpu_val_7_8 - f8;
    result = (fpu_val_7_8 < f8 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_8, f8) ? 1 : 0) << 0xa
        | (fpu_val_7_8 == f8 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x80000;

    long double fpu_val_7_9 = arg2[0x82];
    long double f9 = 0.0f;
    fpu_val_7_9 - f9;
    result = (fpu_val_7_9 < f9 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_9, f9) ? 1 : 0) << 0xa
        | (fpu_val_7_9 == f9 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x100000;

    long double fpu_val_7_10 = arg2[0x83];
    long double f10 = 0.0f;
    fpu_val_7_10 - f10;
    result = (fpu_val_7_10 < f10 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_10, f10) ? 1 : 0) << 0xa
        | (fpu_val_7_10 == f10 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
    {
        dest_2 |= 0x200000;
        arg2[0x343] = arg2[0x83];
    }

    long double fpu_val_7_12 = arg2[0x84];
    long double f11 = 0.0f;
    fpu_val_7_12 - f11;
    result = (fpu_val_7_12 < f11 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_12, f11) ? 1 : 0) << 0xa
        | (fpu_val_7_12 == f11 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
    {
        dest_2 |= &__dos_header;
        result = AI_CheckSector_18(arg2);

        if (result != 0x24)
        {
            arg2[0x344] = arg2[0x84];
            result = AI_UpdateOrders_12(arg2);
            arg2[0x84] = 0;
        }
    }

    long double fpu_val_7_14 = arg2[0x85];
    long double f12 = 0.0f;
    fpu_val_7_14 - f12;
    result = (fpu_val_7_14 < f12 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_14, f12) ? 1 : 0) << 0xa
        | (fpu_val_7_14 == f12 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x800000;

    long double fpu_val_7_15 = arg2[0x86];
    long double f13 = 0.0f;
    fpu_val_7_15 - f13;
    result = (fpu_val_7_15 < f13 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_15, f13) ? 1 : 0) << 0xa
        | (fpu_val_7_15 == f13 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x1000000;

    long double fpu_val_7_16 = arg2[0x87];
    long double f14 = 0.0f;
    fpu_val_7_16 - f14;
    result = (fpu_val_7_16 < f14 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_16, f14) ? 1 : 0) << 0xa
        | (fpu_val_7_16 == f14 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x2000000;

    long double fpu_val_7_17 = arg2[0x88];
    long double f15 = 0.0f;
    fpu_val_7_17 - f15;
    result = (fpu_val_7_17 < f15 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_17, f15) ? 1 : 0) << 0xa
        | (fpu_val_7_17 == f15 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
    {
        int item_8_3 = arg2[0x88];
        int* data_1 = HumanPlayer_UpdateMotion_11(arg2, item_8_3);

        if (!data_1)
        {
            int item_1c_3 = item_8_3;
            int item_20_3 = Script_EvaluateCondition_4(arg2);
            result = Warning_Show("#%d is trying to change to invalid weapon #%d");
        }
        else
            result = Script_ExecuteLevelScript_12(&arg2[0x131], nullptr, data_1);
    }

    long double fpu_val_7_19 = arg2[0x89];
    long double f16 = 0.0f;
    fpu_val_7_19 - f16;
    result = (fpu_val_7_19 < f16 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_19, f16) ? 1 : 0) << 0xa
        | (fpu_val_7_19 == f16 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x4000000;

    long double fpu_val_7_20 = arg2[0x8a];
    long double f17 = 0.0f;
    fpu_val_7_20 - f17;
    result = (fpu_val_7_20 < f17 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_20, f17) ? 1 : 0) << 0xa
        | (fpu_val_7_20 == f17 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
        dest_2 |= 0x8000000;

    *(context + 8) = dest_2;
    return result;
}
