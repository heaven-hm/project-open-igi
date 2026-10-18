// IGI native provenance: AI_UpdateOrders_11 - 0x004590F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004590F0-AI_UpdateOrders_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_11.h"
int AI_UpdateOrders_11(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45bf90;
    return result;
}
