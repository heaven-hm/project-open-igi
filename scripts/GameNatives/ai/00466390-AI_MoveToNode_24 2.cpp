// IGI native provenance: AI_MoveToNode_24 - 0x00466390 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00466390-AI_MoveToNode_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_24.h"
int AI_MoveToNode_24(int context, int arg2, int arg3)

{
    *(context + (arg2 << 2) + 0x74) = arg3;
    return arg3;
}
