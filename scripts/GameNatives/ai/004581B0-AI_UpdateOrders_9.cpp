// IGI native provenance: AI_UpdateOrders_9 - 0x004581B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004581B0-AI_UpdateOrders_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_9.hpp"
int AI_UpdateOrders_9(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CombatTarget_13;
    return result;
}
