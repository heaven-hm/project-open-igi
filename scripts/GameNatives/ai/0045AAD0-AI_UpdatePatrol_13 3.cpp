// IGI native provenance: AI_UpdatePatrol_13 - 0x0045AAD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045AAD0-AI_UpdatePatrol_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_13.h"
int AI_UpdatePatrol_13(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CombatTarget_13;
    return result;
}
