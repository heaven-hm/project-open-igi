// IGI native provenance: AI_AlertAllies_5 - 0x004540D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004540D0-AI_AlertAllies_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_5.hpp"
int AI_AlertAllies_5(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_SelectTarget_5;
    return result;
}
