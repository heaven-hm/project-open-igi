// IGI native provenance: AI_UpdateOrders_10 - 0x004588C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004588C0-AI_UpdateOrders_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_10.h"
int AI_UpdateOrders_10(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45b820;
    return result;
}
