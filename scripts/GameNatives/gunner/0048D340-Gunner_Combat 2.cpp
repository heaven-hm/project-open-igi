// IGI native provenance: Gunner_Combat - 0x0048D340 - CFG score 96.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048D340-Gunner_Combat.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Gunner_Combat.h"
int* Gunner_Combat(long double context , int arg2, int* arg3)

{
    int* source_val = arg3;
    void* data_val = *(arg2 + 0x14);
    char* item_64 = "Gunner Combat - Stand";
    void* dest_1 = source_val[0xa] + 8;
    int item_68 = 0xffffffff;
    int item_6c = arg2;
    int item_4c = 0;
    void* item_70 = dest_1;
    FX_SmokeEmit_17(arg2, item_70);
    void* data_1 = *(arg2 + 0x3de4);

    if (!data_1)
        return AI_CombatTarget_2(arg2, source_val);

    int data_3 = *(arg2 + 0x3ac8);
    void* item_78;

    if (data_3 != 0xffffffff)
    {
        void* data_4 = Script_ExecuteLevelScript_4(data_3);

        if (!data_4)
        {
            int item_64_3 = *(arg2 + 0x3ac8);
            Warning_Show("Stationary gun ID #%d not found");
        label_48d3f5:

            if (data_4)
            {
                double item_38;
                long long* item_68_3 = &item_38;
                AI_UpdatePatrol_3(item_68_3, arg2);
                long double fpu_val_7_2 = item_38 - *(data_4 + 0xf0);
                double item_30;
                long double fpu_val_6_2 = item_30 - *(data_4 + 0xf8);
                double item_30_1 = fpu_val_6_2;
                double item_28;
                long double fpu_val_5_2 = item_28 - *(data_4 + 0x100);
                double item_28_1 = fpu_val_5_2;
                double item_20 = *(data_4 + 0x120) * fpu_val_5_2 + *(data_4 + 0x114) * fpu_val_6_2
                    + *(data_4 + 0x108) * fpu_val_7_2;
                double item_18_1 = *(data_4 + 0x124) * fpu_val_5_2 + *(data_4 + 0x118) * fpu_val_6_2
                    + *(data_4 + 0x10c) * fpu_val_7_2;
                double item_10_1 = *(data_4 + 0x128) * fpu_val_5_2 + *(data_4 + 0x11c) * fpu_val_6_2
                    + *(data_4 + 0x110) * fpu_val_7_2;
                memcpy(&item_38, &item_20, 0x18);
                item_38 = -(item_20);
                double item_30_2 = -(item_30_1);
                double item_28_2 = -(item_28_1);
                memcpy(&item_78, &item_38, 0x18);
                WeaponType_InitType();
                long double fpu0;
                float item_50 = fpu0;
                memcpy(&item_78, &item_38, 0x18);
                WeaponType_GetConfig();
                float item_54_1 = context;
                item_68_3 = context;
                item_70 = item_50;
                char* item_74_1 = "Gun->Tgt: A:%1.3f, G:%1.3f";
                item_78 = 0xffffffff;
                int item_7c_1 = arg2;
                char* item_80_1 = "Gun params:";
                int item_84_1 = 0xffffffff;
                int item_88_1 = arg2;
                item_68_3 = *(data_4 + 0x1e0) * 0.0174532924f;
                item_70 = *(data_4 + 0x1e4) * 0.0174532924f;
                item_78 = *(data_4 + 0x1e8) * 0.0174532924f;
                char* item_7c_2 = "MG:%1.3f, A-:%1.3f, A+:%1.3f";
                int item_80_2 = 0xffffffff;
                int item_84_2 = arg2;
                long double fpu_val_1_7 = item_54_1;
                long double f1_1 = 0.0f;
                fpu_val_1_7 - f1_1;
                long double fpu_val_1_8 = item_54_1;

                if (*(&((fpu_val_1_7 < f1_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_1_7, f1_1) ? 1 : 0) << 0xa
                        | (fpu_val_1_7 == f1_1 ? 1 : 0) << 0xe | 0x800) + 1) & 1)
                    fpu_val_1_8 = -(fpu_val_1_8);

                long double fpu_val_0_2 = *(data_4 + 0x1e8) * 0.0174532924f;
                fpu_val_1_8 - fpu_val_0_2;

                if (*(&((fpu_val_1_8 < fpu_val_0_2 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_1_8, fpu_val_0_2) ? 1 : 0) << 0xa
                    | (fpu_val_1_8 == fpu_val_0_2 ? 1 : 0) << 0xe | 0x1000) + 1) & 1)
                {
                    long double fpu_val_1_11 = *(data_4 + 0x1e4) * 0.0174532924f;
                    long double f2_1 = item_50;
                    fpu_val_1_11 - f2_1;

                    if (*(&((fpu_val_1_11 < f2_1 ? 1 : 0) << 8
                        | (FCMP_UO(fpu_val_1_11, f2_1) ? 1 : 0) << 0xa
                        | (fpu_val_1_11 == f2_1 ? 1 : 0) << 0xe | 0x1000) + 1) & 1)
                    {
                        long double fpu_val_1_13 = *(data_4 + 0x1e0) * 0.0174532924f;
                        long double f3_1 = item_50;
                        fpu_val_1_13 - f3_1;

                        if (!(*(&((fpu_val_1_13 < f3_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_1_13, f3_1) ? 1 : 0) << 0xa
                            | (fpu_val_1_13 == f3_1 ? 1 : 0) << 0xe | 0x1000) + 1) & 0x41))
                        {
                            char* item_64_5 = "Target is in guncone";
                            int item_68_4 = 0xffffffff;
                            int item_6c_1 = arg2;
                            item_4c = 1;
                        }
                    }
                }
            }
        }
        else if (!*(data_4 + 0x21c) || *(data_4 + 0x208) == data_val)
            goto label_48d3f5;

        source_val = arg3;
    }

    char data_9;
    int entity_2;
    data_9 = Locomotion_GetState(arg2, 4);

    if (!data_9)
    {
        int data_30 = item_4c;

        if (!data_30)
            *(dest_1 + 0x14) = 0x1e;
        else
        {
            int data_31 = *(dest_1 + 0x14);

            if (!data_31)
            {
🚫🚫                /* unimplemented  {fild fp0, dword [context_val+0x3ac8]} */
                int item_64_16 = entity_2;
🚫🚫🚫                float item_64_17 = /* float item_64_17 = unimplemented  {fstp dword [stack_ptr], fp0} */;
                /* unimplemented  {fstp dword [stack_ptr], fp0} */
                return AI_WriteChannel(arg2, 4, item_64_17);
            }

            data_30 = data_31 - 1;
            *(dest_1 + 0x14) = data_30;
        }

        if (*(arg2 + 0x3e00) != 0xffffffff)
            return Guard_CombatStand(arg2, source_val);

        char* item_64_18 = "No primary weapon";
        int item_68_16 = 0xffffffff;
        int item_6c_9 = arg2;
        return data_30;
    }

    int entity_3 = *(dest_1 + 0x18);
    *(dest_1 + 0x18) = 0;

    if (item_4c)
        *(dest_1 + 0x14) = 0x1e;
    else
    {
        int data_11 = *(dest_1 + 0x14);

        if (!data_11)
        {
            AI_WriteChannel(arg2, 4, 1.0f);
            int item_70_1 = 0xffffffff;
            item_78 = *(arg2 + 0x3aa8);
            return Combat_SetAimMode(item_78, arg2, item_70_1);
        }

        *(dest_1 + 0x14) = data_11 - 1;
    }

    Weapon_FireBullet_4(**(data_val + 0x7b4), arg2 + 0x38, arg2 + 0x78);

    if (!*(data_1 + 0x29))
    {
        Combat_SetAimMode(*(arg2 + 0x3aa8), arg2, 0xffffffff);
        *(dest_1 + 0x18) = entity_3 + 1;

        if (entity_3 + 1 <= *(arg2 + 0x3d50))
        {
            source_val[9] = 0;
            return AI_UpdateOrders_3(arg2, *(arg2 + 0x3aa8) + 0x110, 0.0349065848f, 0);
        }

        return AI_CombatTarget_2(arg2, source_val);
    }

    int data_22 = source_val[9];

    if (data_22)
    {
        int* data_23 = data_22(arg2, source_val);

        if (!data_23)
            return data_23;

        source_val[9] = 0;
    }

    if (AI_UpdateOrders_2(*(arg2 + 0x3aa8), 0) >= 4)
        return Combat_SetAimMode(*(arg2 + 0x3aa8), arg2, 2);

    Combat_SetAimMode(*(arg2 + 0x3aa8), arg2, 0);
    void* entity_9 = AI_UpdatePatrol_3(arg2 + 0x3e88, arg2);
    entity_9 = *(source_val[0xa] + 0x28);
    item_78 = entity_9;
    unsigned int data_27 = Combat_BeginBurst(arg2, item_78);
    *(source_val[0xa] + 0x28) = 0;
    source_val[9] = StandFire;
    return data_27;
}
