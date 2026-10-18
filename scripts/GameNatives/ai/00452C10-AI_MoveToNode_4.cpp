// IGI native provenance: AI_MoveToNode_4 - 0x00452C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00452C10-AI_MoveToNode_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_4.hpp"
void* AI_MoveToNode_4(int* context, int arg2, int arg3)

{
    context[4] -= 1;
    AI_CheckHearing_4(context, arg2, arg3, 0);
    void* result = context[4];

    if (result)
        return result;

    HeapList_Remove(context);
    context[1] = 0;
    *context = 0;
    return HeapList_Insert(&g_58a9e8, context);
}
