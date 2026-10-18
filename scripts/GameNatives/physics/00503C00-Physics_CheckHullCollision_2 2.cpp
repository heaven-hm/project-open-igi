// IGI native provenance: Physics_CheckHullCollision_2 - 0x00503C00 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00503C00-Physics_CheckHullCollision_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CheckHullCollision_2.h"
struct status_msg* Physics_CheckHullCollision_2(unsigned int context, int arg2, int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6, unsigned int arg7, int* arg8, char arg9)

{
    int kind;
    kind = g_a7a628;
    struct status_msg* result = QTask_HandleInput(Script_FormatString(), kind, 0);
    int dest_val = arg2;
    result->head[8] = context;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_1 = ~i;
    int source_1;
    int dest_3;
    dest_3 = memcpy(&result->head[9], dest_val - entity_1, entity_1 & 0xfffffffc);
    memcpy(dest_3, source_1, entity_1 & 3);
    int dest_4 = arg3;

    if (!dest_4)
        result->head[0xd] = 0;
    else
    {
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_1_1 = 0 != *dest_4;
            dest_4 += 1;
            i_1 -= 1;

            if (!flag_1_1)
                break;
        }

        int entity_5 = ~i_1;
        int source_3;
        int dest_7;
        dest_7 = memcpy(&result->head[0xd], dest_4 - entity_5, entity_5 & 0xfffffffc);
        memcpy(dest_7, source_3, entity_5 & 3);
    }

    result->head[0x12] = arg5;
    result->head[0x11] = arg4;
    result->head[0x14] = arg7;
    result->head[0x13] = arg6;
    result->head[0x15] = *arg8;
    result->head[0x16] = arg8[1];
    result->head[0x17] = arg8[2];
    result->head[0x18] = arg9;
    return result;
}
