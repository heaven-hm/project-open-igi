// IGI native provenance: AI_MoveToNode_26 - 0x00469430 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00469430-AI_MoveToNode_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_26.hpp"
struct status_msg* AI_MoveToNode_26(void* context)

{
    int kind;
    kind = g_5bdd70;
    return QTask_HandleInput(context, kind, 0);
}
