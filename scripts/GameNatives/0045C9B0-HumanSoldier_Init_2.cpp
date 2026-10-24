// IGI native provenance: HumanSoldier_Init_2 - 0x0045C9B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045C9B0-HumanSoldier_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045C9B0-HumanSoldier_Init_2.hpp"
void* HumanSoldier_Init_2(void* self, void* args)

{
    void* result = AI_CombatTarget_21(args);
    long double fpu_val_7 = *(args + 0x210);
    long double temp0 = 0.0f;
    fpu_val_7 - temp0;
    void* result_1 = result;
    result = (fpu_val_7 < temp0 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7, temp0) ? 1 : 0) << 0xa
        | (fpu_val_7 == temp0 ? 1 : 0) << 0xe;

    if (*(&result + 1) & 0x40)
    {
    label_45c9ec:

        if (!(*(self + 8) & 0x20000))
            return result;
    }
    else
    {
        long double fpu_val_7_1 = *(args + 0x170);
        long double temp1_1 = 0.0f;
        fpu_val_7_1 - temp1_1;
        result = (fpu_val_7_1 < temp1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, temp1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == temp1_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x40))
            goto label_45c9ec;
    }

    AI_TakeCover_22(*(args + 0x4ec));
    AI_UpdateOrders_20(args);
    Weapon_ReloadClip_4(result_1);
    AI_SelectTarget_20(args);
    AI_UpdatePatrol_21(args, nullptr);
    AI_CheckSector_19(args);
    int* scratch_val_1 = *(self + 4);
    *(self + 0xc) = *(self + 0x10);
    *(self + 0x10) = 0;
    int result_val_1 = *scratch_val_1;
    *(self + 0x14) = result_val_1;
    return result_val_1(self, args);
}
