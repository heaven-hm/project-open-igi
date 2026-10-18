// IGI native provenance: AI_MoveToNode_15 - 0x0045BF60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045BF60-AI_MoveToNode_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045BF60-AI_MoveToNode_15.hpp"
int AI_MoveToNode_15(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45bf90;
    return result;
}
