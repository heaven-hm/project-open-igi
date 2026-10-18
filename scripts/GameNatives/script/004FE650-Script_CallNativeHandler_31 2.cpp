// IGI native provenance: Script_CallNativeHandler_31 - 0x004FE650 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004FE650-Script_CallNativeHandler_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_CallNativeHandler_31.h"
struct status_msg* Script_CallNativeHandler_31(void* context, int arg2, int arg3)

{
    int kind;
    kind = g_a7a4e0;
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
    struct status_msg* result_1 = result;
    int source_1;
    int dest_3;
    dest_3 = memcpy(&result->head[8], dest_val - entity_1, entity_1 & 0xfffffffc);
    memcpy(dest_3, source_1, entity_1 & 3);
    result->sprite[0x70] = arg3;
    result->sprite[0x71] = *(&arg3 + 1);
    result->sprite[0x72] = *(&arg3 + 2);
    result->sprite[0x73] = *(&arg3 + 3);
    return result;
}
