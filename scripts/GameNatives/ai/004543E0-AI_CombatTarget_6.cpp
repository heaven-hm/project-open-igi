// IGI native provenance: AI_CombatTarget_6 - 0x004543E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004543E0-AI_CombatTarget_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_6.hpp"
int AI_CombatTarget_6(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_454410;
    return result;
}
