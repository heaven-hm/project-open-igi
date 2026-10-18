// IGI native provenance: Combat_BeginBurst - 0x0048B9C0 - CFG score 97.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048B9C0-Combat_BeginBurst.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Combat_BeginBurst.h"
unsigned int Combat_BeginBurst(int context, char arg2)

{
    void* context_val = *(context + 0x14);
    float item_10 = Sys_AllocateMemory_14(Level_SetMission_5()) * *(context + 0x3d74) + *(context + 0x3d70);
    char data_1;
    void* temp_val;
    data_1 = Locomotion_GetState(context, 4);
    void* data_3;
    int entity_1;

    if (!data_1)
        data_3 = Table_GetAt(context_val + 0x4c4, 0);
    else
    {
        temp_val = context + 0x38;
        int source_1 = **(context_val + 0x7b4);
        Weapon_FireBullet_4(source_1, temp_val, context + 0x78);
        data_3 = Table_GetAt(source_1 + 0x20c, 0);
    }

    void* data_5 = HumanPlayer_LoadParameters_13(data_3, temp_val, entity_1, data_3);
    int item_8;
    Script_ParseEnumReal32_13(data_3, &item_8);
    int data_6 = *(data_5 + 0x610);

    if (data_6 >= 0)
    {
        if (data_6 <= 2)
        {
            int data_7 = *(data_5 + 0x64c);
            int flagP1 = *(data_5 + 0x648);

            if (data_7 == flagP1 || data_7 <= 1)
            {
                item_10 = flagP1 * item_10;
                *(context + 0x3e54) = item_10;

                if (item_10 >= 1)
                {
                    int entity_2 = *(context + 0x3d4c);

                    if (item_10 > entity_2)
                        *(context + 0x3e54) = entity_2;
                }
                else
                    *(context + 0x3e54) = 1;
            }
            else
                *(context + 0x3e54) = data_7;

            if (!*(context + 0x3e54))
                *(context + 0x3e54) = 1;

            long double fpu_val_7_5 = Sys_DispatchOpcode_11(context + 0x20, context + 0x3e88);
            long double f1_1 = 0.0;
            fpu_val_7_5 - f1_1;
            short data_12 = (fpu_val_7_5 < f1_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_5, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_5 == f1_1 ? 1 : 0) << 0xe | 0x3800;

            if (*(&data_12 + 1) & 1)
                *(context_val + 0x4d4) = *(context + 0x3d68);
            else
            {
                long double fpu_val_6_1 = *(context + 0xb0);
                fpu_val_7_5 - fpu_val_6_1;

                if (!(*(&((fpu_val_7_5 < fpu_val_6_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_7_5, fpu_val_6_1) ? 1 : 0) << 0xa
                        | (fpu_val_7_5 == fpu_val_6_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
                    fpu_val_7_5 = *(context + 0xb0);

                long double f2_1 = 0.01;
                fpu_val_7_5 - f2_1;
                data_12 = (fpu_val_7_5 < f2_1 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_7_5, f2_1) ? 1 : 0) << 0xa
                    | (fpu_val_7_5 == f2_1 ? 1 : 0) << 0xe | 0x3800;

                if (!(*(&data_12 + 1) & 1))
                    *(context_val + 0x4d4) =
                        (*(context + 0x3d6c) - *(context + 0x3d68)) * fpu_val_7_5 / *(context + 0xb0)
                        + *(context + 0x3d68);
                else
                    *(context_val + 0x4d4) = *(context + 0x3d68);
            }

            data_12 = arg2;

            if (data_12)
                *(context_val + 0x4d4) = *(context_val + 0x4d4) * 0.5f;
        }
        else if (data_6 == 4)
        {
            *(context + 0x3e54) = 1;
            *(context_val + 0x4d4) = 0x3f800000;
        }
    }

    *(context + 0x3e58) = item_8;
    unsigned int result = Frames_GetUpdateCounter();
    *(context + 0x3e5c) = result;
    *(context + 0x3e60) = 0;
    *(context + 0x3e6a) = 0;
    *(context + 0x3e69) = 1;
    memcpy(context + 0x3e70, context + 0x3e88, 0x18);
    return result;
}
