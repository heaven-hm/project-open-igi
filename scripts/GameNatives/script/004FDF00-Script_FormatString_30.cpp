// IGI native provenance: Script_FormatString_30 - 0x004FDF00 - CFG score 97.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FDF00-Script_FormatString_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FDF00-Script_FormatString_30.hpp"
struct status_msg* Script_FormatString_30(void* context, void* arg2, unsigned int arg3, int* arg4, int* arg5, int* arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12, int arg13)

{
    int kind;
    kind = g_54e568;
    int item_18;
    memset(&item_18, 0, 0x18);
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    DynObj_LookupHashItem_9(result);
    DynObj_LinkDynamicCube_8(arg2, result, &item_18, 4096.0f, 2);
    result->sprite[0x44] = arg12;
    result->sprite[0x45] = *(&arg12 + 1);
    result->sprite[0x46] = *(&arg12 + 2);
    result->sprite[0x47] = *(&arg12 + 3);
    result->sprite[4] = arg2;
    result->sprite[5] = *(&arg2 + 1);
    result->sprite[6] = *(&arg2 + 2);
    result->sprite[7] = *(&arg2 + 3);
    int data_1 = *arg4;
    result->sprite[0x10] = data_1;
    result->sprite[0x11] = *(&data_1 + 1);
    result->sprite[0x12] = *(&data_1 + 2);
    result->sprite[0x13] = *(&data_1 + 3);
    int data_2 = arg4[1];
    result->sprite[0x14] = data_2;
    result->sprite[0x15] = *(&data_2 + 1);
    result->sprite[0x16] = *(&data_2 + 2);
    result->sprite[0x17] = *(&data_2 + 3);
    int temp_1 = arg4[2];
    result->sprite[0x18] = temp_1;
    result->sprite[0x19] = *(&temp_1 + 1);
    result->sprite[0x1a] = *(&temp_1 + 2);
    result->sprite[0x1b] = *(&temp_1 + 3);
    int temp_2 = *arg5;
    result->sprite[0x1c] = temp_2;
    result->sprite[0x1d] = *(&temp_2 + 1);
    result->sprite[0x1e] = *(&temp_2 + 2);
    result->sprite[0x1f] = *(&temp_2 + 3);
    int temp_3 = arg5[1];
    result->sprite[0x20] = temp_3;
    result->sprite[0x21] = *(&temp_3 + 1);
    result->sprite[0x22] = *(&temp_3 + 2);
    result->sprite[0x23] = *(&temp_3 + 3);
    int entity_3 = arg5[2];
    result->sprite[0x24] = entity_3;
    result->sprite[0x25] = *(&entity_3 + 1);
    result->sprite[0x26] = *(&entity_3 + 2);
    result->sprite[0x27] = *(&entity_3 + 3);
    int temp_4 = *arg6;
    result->sprite[8] = temp_4;
    result->sprite[9] = *(&temp_4 + 1);
    result->sprite[0xa] = *(&temp_4 + 2);
    result->sprite[0xb] = *(&temp_4 + 3);
    int data_5 = arg6[1];
    result->sprite[0xc] = data_5;
    result->sprite[0xd] = *(&data_5 + 1);
    result->sprite[0xe] = *(&data_5 + 2);
    result->sprite[0xf] = *(&data_5 + 3);
    result->sprite[0x2c] = arg7;
    result->sprite[0x2d] = *(&arg7 + 1);
    result->sprite[0x2e] = *(&arg7 + 2);
    result->sprite[0x2f] = *(&arg7 + 3);
    result->sprite[0x40] = arg9;
    result->sprite[0x41] = *(&arg9 + 1);
    result->sprite[0x42] = *(&arg9 + 2);
    result->sprite[0x43] = *(&arg9 + 3);
    int temp_6 = arg8 | 0x40000;
    result->sprite[0x34] = temp_6;
    result->sprite[0x35] = *(&temp_6 + 1);
    result->sprite[0x36] = *(&temp_6 + 2);
    result->sprite[0x37] = *(&temp_6 + 3);
    result->sprite[0x30] = arg10;
    result->sprite[0x31] = *(&arg10 + 1);
    result->sprite[0x32] = *(&arg10 + 2);
    result->sprite[0x33] = *(&arg10 + 3);
    result->sprite[0x48] = 0;
    result->head[0x20] = arg3;

    if (*(arg3 + 0xc))
        result->sprite[0x48] = 1;

    result->sprite[0x28] = arg11;
    result->sprite[0x29] = *(&arg11 + 1);
    result->sprite[0x2a] = *(&arg11 + 2);
    result->sprite[0x2b] = *(&arg11 + 3);
    result->sprite[0x38] = arg13;
    result->sprite[0x39] = *(&arg13 + 1);
    result->sprite[0x3a] = *(&arg13 + 2);
    result->sprite[0x3b] = *(&arg13 + 3);
    Qhash_Init(1);
    int data_9;
    data_9 = result->sprite[0x44];
    *(&data_9 + 1) = result->sprite[0x45];
    *(&data_9 + 2) = result->sprite[0x46];
    *(&data_9 + 3) = result->sprite[0x47];
    result->head[0x1a] = Mem_Alloc(data_9 * 0x58, 4);
    int data_11;
    data_11 = result->sprite[0x44];
    *(&data_11 + 1) = result->sprite[0x45];
    *(&data_11 + 2) = result->sprite[0x46];
    *(&data_11 + 3) = result->sprite[0x47];
    void* data_13 = Mem_Alloc(data_11 << 2, 4);
    int entity_8;
    entity_8 = result->sprite[0x44];
    *(&entity_8 + 1) = result->sprite[0x45];
    *(&entity_8 + 2) = result->sprite[0x46];
    *(&entity_8 + 3) = result->sprite[0x47];
    result->head[0x1b] = data_13;
    result->head[0x1f] = Mem_Alloc(entity_8 << 2, 4);
    int data_15;
    data_15 = result->sprite[0x44];
    *(&data_15 + 1) = result->sprite[0x45];
    *(&data_15 + 2) = result->sprite[0x46];
    *(&data_15 + 3) = result->sprite[0x47];
    result->head[0x1c] = Mem_Alloc(data_15 * 0x70, 4);
    int data_17;
    data_17 = result->sprite[0x44];
    *(&data_17 + 1) = result->sprite[0x45];
    *(&data_17 + 2) = result->sprite[0x46];
    *(&data_17 + 3) = result->sprite[0x47];
    result->head[0x1d] = Mem_Alloc(data_17 * 0x18, 4);
    int data_21;
    data_21 = result->sprite[0x44];
    *(&data_21 + 1) = result->sprite[0x45];
    *(&data_21 + 2) = result->sprite[0x46];
    *(&data_21 + 3) = result->sprite[0x47];
    result->head[0x1e] = Mem_Alloc(data_21 * 0x18, 4);
    Qhash_Reset();
    int entity_12;
    entity_12 = result->sprite[0x44];
    *(&entity_12 + 1) = result->sprite[0x45];
    *(&entity_12 + 2) = result->sprite[0x46];
    *(&entity_12 + 3) = result->sprite[0x47];
    int data_23 = 0;

    if (entity_12 > 0)
    {
        int entity_13 = 0;
        int temp_15;

        do
        {
            int temp_14 = entity_13 + result->head[0x1a];
            entity_13 += 0x58;
            *(result->head[0x1b] + (data_23 << 2)) = temp_14;
            temp_15 = result->sprite[0x44];
            *(&temp_15 + 1) = result->sprite[0x45];
            *(&temp_15 + 2) = result->sprite[0x46];
            *(&temp_15 + 3) = result->sprite[0x47];
            data_23 += 1;
        } while (data_23 < temp_15);
    }

    int data_24;
    data_24 = result->sprite[0x44];
    *(&data_24 + 1) = result->sprite[0x45];
    *(&data_24 + 2) = result->sprite[0x46];
    *(&data_24 + 3) = result->sprite[0x47];
    int data_26 = g_a7a4d0 + data_24 * 0x58 + 0x100;
    bool flag_0 = g_a7a4d4 > data_26;
    g_a7a4d0 = data_26;

    if (!flag_0)
        g_a7a4d4 = data_26;

    return result;
}
