// IGI native provenance: AI_MoveToNode_11 - 0x00458FE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00458FE0-AI_MoveToNode_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_11.hpp"
int AI_MoveToNode_11(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45bdc0;
    return result;
}
