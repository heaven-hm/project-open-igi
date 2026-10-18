// IGI native provenance: AI_AlertAllies_16 - 0x0045DAA0 - CFG score 85.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045DAA0-AI_AlertAllies_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_16.h"
int AI_AlertAllies_16(long double context , void* arg2, double* arg3)

{
    if (Sys_PopStack_11(arg3))
        return 1;

    double item_18;
    memcpy(&item_18, arg3, 0x18);
    Sys_FreeMemory_11(&item_18);
    double item_10;
    double item_8;
    long double fpu_val_4_3 =
        item_8 * *(arg2 + 0x94) + item_10 * *(arg2 + 0x88) + item_18 * *(arg2 + 0x7c);
    long double f0 = 0.0;
    fpu_val_4_3 - f0;
    void item_40;
    bool c1;

    if (*(&((fpu_val_4_3 < f0 ? 1 : 0) << 8 | (c1 ? 1 : 0) << 9
        | (FCMP_UO(fpu_val_4_3, f0) ? 1 : 0) << 0xa | (fpu_val_4_3 == f0 ? 1 : 0) << 0xe | 0x3000) + 1)
        & 0x41)
    {
        memcpy(&item_40, &item_18, 0x18);
        int data_4 = WeaponType_GetConfig();
        data_4 = 1;
        *(arg2 + 0x50c) = context + 3.14159274f;
        return data_4;
    }

    memcpy(&item_40, &item_18, 0x18);
    int data_3 = WeaponType_GetConfig();
    *(arg2 + 0x50c) = context;
    data_3 = 0;
    return data_3;
}
