// IGI native provenance: AI_CombatTarget_15 - 0x0045BC90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045BC90-AI_CombatTarget_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_15.hpp"
int AI_CombatTarget_15(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CheckVisibility_15;
    return result;
}
