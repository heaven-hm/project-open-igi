// IGI native provenance: AI_UpdatePatrol_14 - 0x0045B500 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045B500-AI_UpdatePatrol_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_14.hpp"
int AI_UpdatePatrol_14(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    *(arg2 + 0xd19) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CombatTarget_14;
    return result;
}
