// IGI native provenance: AI_MoveToNode_8 - 0x00456350 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456350-AI_MoveToNode_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_8.h"
int AI_MoveToNode_8(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_TakeCover_8;
    return result;
}
