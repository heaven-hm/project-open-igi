// IGI native provenance: QTask_HandleInput - 0x004012A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004012A0-QTask_HandleInput.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_HandleInput.h"
struct status_msg* QTask_HandleInput(void* ctx, int kind, int flags)

{
    int* result = QTask_LinkEventHandler(ctx, kind);

    if (ctx)
        HeapList_Insert(ctx + 8, result);

    int entity_val = 0;
    entity_val = result[7];
    int data_val = *((entity_val << 2) + &g_a96ae0);

    if (data_val)
        data_val(result, flags);

    return result;
}
