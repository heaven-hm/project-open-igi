// IGI native provenance: Explosion0_Cleanup - 0x00480420 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00480420-Explosion0_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Explosion0_Cleanup.h"
int Explosion0_Cleanup(int context)

{
    int* source_val = context;
    int data_val = source_val[8];

    if (data_val <= 0)
        return QTask_FindType(source_val);

    int entity_val = source_val[9];
    long long item_8;

    if (data_val == entity_val)
    {
        item_8 = entity_val * 0.100000001f;
        short dest_1 = item_8;
        context = source_val[9];
        long double fpu_val_7_5 = Sys_AllocateMemory_14(Level_SetMission_5()) * 0.200000003f;
        int data_3 = source_val[0x10];
        item_8 = data_3;
        *(&item_8 + 4) = data_3;
        Weapon_EjectShell_9(source_val, 0x1000008, &item_8, 1.0f / context, dest_1, (fpu_val_7_5 + 0.800000012f) * context,
            (Sys_AllocateMemory_14(Level_SetMission_5()) + 1.0f) * 0.100000001f);
    }

    struct status_msg* data_7 = source_val[9];
    int entity_2 = source_val[8];
    long double fpu_val_7_13 = data_7;
    item_8 = fpu_val_7_13 * 0.899999976f;

    if (entity_2 == item_8)
    {
        int entity_3 = source_val[0x10];
        item_8 = entity_3;
        *(&item_8 + 4) = entity_3;
        source_val[0x11] = Weapon_EjectShell_9(source_val, 0x1000010, &item_8, 1.0f / fpu_val_7_13, data_7, 0, 0x3a83126f);
    }

    void* saved_base_4 = source_val[0x11];

    if (saved_base_4)
    {
        long double fpu_val_7_16 = source_val[8];
        long double fpu_val_6_4 = source_val[9];
        long double fpu_val_6_5 = fpu_val_6_4 * 0.5f;
        fpu_val_7_16 - fpu_val_6_5;
        long double fpu_val_6_9;

        fpu_val_6_9 = *(&((fpu_val_7_16 < fpu_val_6_5 ? 1 : 0) << 8
            | (FCMP_UO(fpu_val_7_16, fpu_val_6_5) ? 1 : 0) << 0xa
            | (fpu_val_7_16 == fpu_val_6_5 ? 1 : 0) << 0xe | 0x3000) + 1) & 0x41 ? fpu_val_7_16 / fpu_val_6_5
            : 1.0f - (fpu_val_7_16 - fpu_val_6_5) / (fpu_val_6_4 * 0.899999976f - fpu_val_6_5);

        void* saved_base_2 = saved_base_4;
        Script_ParseEnumString_31(saved_base_4, fpu_val_6_9 * 0.300000012f);
    }

    int result = source_val[8] - 1;
    source_val[8] = result;
    return result;
}
