// IGI native provenance: TerraDrawTask_OnEvent - 0x004E8370 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8370-TerraDrawTask_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "TerraDrawTask_OnEvent.h"
struct status_msg* TerraDrawTask_OnEvent(void* context, unsigned int arg2)

{
    int kind;
    kind = g_54d9a0;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    result->head[8] = arg2;
    return result;
}
