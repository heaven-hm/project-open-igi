// IGI native provenance: AI_MoveToNode_12 - 0x00459850 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00459850-AI_MoveToNode_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_12.h"
int AI_MoveToNode_12(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45ca80;
    return result;
}
