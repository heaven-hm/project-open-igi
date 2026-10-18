// IGI native provenance: Script_EvaluateCondition_31 - 0x004FE6B0 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004FE6B0-Script_EvaluateCondition_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_EvaluateCondition_31.h"
struct status_msg* Script_EvaluateCondition_31(void* context, int arg2, int arg3)

{
    int kind;
    kind = g_a7a4fe;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    int dest_val = arg2;
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
    int item_20 = arg2;
    int source_1;
    int dest_3;
    dest_3 = memcpy(&result->head[8], dest_val - entity_1, entity_1 & 0xfffffffc);
    struct status_msg* result_1 = result;
    memcpy(dest_3, source_1, entity_1 & 3);
    int dest_4 = arg3;
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
    dest_7 = memcpy(&result->sprite[0x70], dest_4 - entity_5, entity_5 & 0xfffffffc);
    memcpy(dest_7, source_3, entity_5 & 3);
    return result;
}
