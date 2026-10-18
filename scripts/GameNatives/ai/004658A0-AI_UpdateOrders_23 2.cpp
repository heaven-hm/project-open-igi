// IGI native provenance: AI_UpdateOrders_23 - 0x004658A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004658A0-AI_UpdateOrders_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_23.h"
int AI_UpdateOrders_23(int* context, void* arg2)

{
    *context = *(arg2 + 0xfc);
    context[1] = *(arg2 + 0x108);
    int result = *(arg2 + 0x114);
    context[2] = result;
    return result;
}
