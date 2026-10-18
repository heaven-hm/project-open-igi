// IGI native provenance: AI_MoveToNode_3 - 0x004522A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004522A0-AI_MoveToNode_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_3.hpp"
int* AI_MoveToNode_3(void* context, int arg2, double* arg3, char arg4)

{
    *(context + 0x3ec8) = 0x40490fdb;
    *(context + 0x3ecc) = 0x40490fdb;
    return AI_ExpandRoute(context, arg2, arg3, arg4);
}
