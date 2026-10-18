// IGI native provenance: FX_DebrisSpawn - 0x004810A0 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 004810A0-FX_DebrisSpawn.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004810A0-FX_DebrisSpawn.hpp"
struct status_msg* FX_DebrisSpawn(int context, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6, int arg7, unsigned int arg8, unsigned int arg9, int* arg10, int arg11, int arg12, int* arg13, int arg14, int arg15, int* arg16, unsigned int arg17, unsigned int arg18, unsigned int arg19, unsigned int arg20)

{
    int kind;
    kind = g_5c1154;
    struct status_msg* result = QTask_HandleInput(Script_FormatString(), kind, 0);
    memcpy(&result->head[8], context, 0x18);
    result->head[0xf] = arg3;
    result->head[0xe] = arg2;
    result->head[0x10] = arg3;
    result->head[0x14] = arg4;
    result->head[0x16] = arg6;
    result->head[0x15] = arg5;
    result->head[0x11] = *arg16;
    result->head[0x12] = arg16[1];
    result->head[0x13] = arg16[2];
    long double fpu7 = result->head[0x11];
    long double f0 = 0.0f;
    fpu7 - f0;

    if (!(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40))
    {
    label_48113e:
        long double fpu_val_7_3 = result->head[0x13];
        long double fpu_val_6_1 = result->head[0x12];
        long double fpu_val_5_1 = result->head[0x11];
        long double fpu_val_7_5 =
            1.0 / sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_3 * fpu_val_7_3);
        result->head[0x11] = result->head[0x11] * fpu_val_7_5;
        result->head[0x12] = result->head[0x12] * fpu_val_7_5;
        result->head[0x13] = result->head[0x13] * fpu_val_7_5;
    }
    else
    {
        long double fpu_val_7_1 = result->head[0x12];
        long double f1_1 = 0.0f;
        fpu_val_7_1 - f1_1;

        if (!(*(&((fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_48113e;

        long double fpu_val_7_2 = result->head[0x13];
        long double f2_1 = 0.0f;
        fpu_val_7_2 - f2_1;

        if (!(*(&((fpu_val_7_2 < f2_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_2 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            goto label_48113e;
    }

    result->head[0x17] = arg17;
    result->head[0x18] = arg18;
    result->head[0x19] = arg19;
    result->head[0x1a] = arg8;
    result->head[0x1b] = arg9;
    int* item_1c;

    if (arg10 && *arg10)
    {
        unsigned int data_8 = Weapon_CheckAmmo_8();
        int entity_5 = 0;
        entity_5 = *(*arg10 + 0x1c);

        if (*(((data_8 * 0x180 + entity_5) << 2) + &g_a96ae0))
        {
            int temp_8 = g_5c1164 + 0x18;
            item_1c = arg10;
            int source_4 = *arg10;
            int item_14_1 = temp_8;
            int item_18_1 = 0;
            int item_10_1 = arg7;
            unsigned int data_13 = Weapon_CheckAmmo_8();
            int temp_9 = 0;
            temp_9 = *(source_4 + 0x1c);
            (*(((data_13 * 0x180 + temp_9) << 2) + &g_a96ae0))(source_4, &item_1c);
        }
    }

    int dest_1 = arg12;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_1;
        dest_1 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_9 = ~i;
    int source_6;
    int dest_4;
    dest_4 = memcpy(&result->head[0x1c], dest_1 - entity_9, entity_9 & 0xfffffffc);
    memcpy(dest_4, source_6, entity_9 & 3);
    float item_28 = Sys_AllocateMemory_14(Level_SetMission_5()) * 0.628318548f;
    float item_24 = Sys_AllocateMemory_14(Level_SetMission_5()) * 0.628318548f;
    float item_20 = Sys_AllocateMemory_14(Level_SetMission_5()) * 0.628318548f;
    float* item_40_2 = &item_28;
    result->head[0x20] = Physics_CheckHullCollision_2(0x2d, arg11, 0, 0x3f7ae148, 0x3f19999a, 0x4232674f,
        result->head[0x15] * 0.000219726557f, &item_28, 0);

    if (arg13 == 0xffffffff)
    {
        result->head[0x21] = arg20;
        return result;
    }

    item_1c = arg13;
    int item_14_2 = arg14;
    int item_18_2 = 1;
    int item_c = context;
    int item_10_2 = arg15;
    int item_4 = 0;
    struct status_msg* result_1 = result;
    AI_EventBroadcast(&item_1c);
    result->head[0x21] = arg20;
    return result;
}
