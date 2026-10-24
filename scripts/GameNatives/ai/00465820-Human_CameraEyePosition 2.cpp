// IGI native provenance: Human_CameraEyePosition - 0x00465820 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00465820-AI_MoveToNode_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_MoveToNode_23.h"
int Human_CameraEyePosition(int* context, void* arg2)

{
    context[4] = 0xe147ae14;
    context[5] = 0x40a6147a;

    if (AI_MoveToNode_19(arg2))
    {
        strncpy(&context[4], "ffff", 4);
        context[5] = 0x40966666;
    }

    *context = *(arg2 + 0x20);
    context[1] = *(arg2 + 0x24);
    context[2] = *(arg2 + 0x28);
    int result = *(arg2 + 0x2c);
    context[3] = result;
    *(context + 0x10) = *(arg2 + 0x30) + *(context + 0x10);
    return result;
}
