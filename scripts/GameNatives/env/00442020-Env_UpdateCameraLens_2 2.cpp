// IGI native provenance: Env_UpdateCameraLens_2 - 0x00442020 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00442020-Env_UpdateCameraLens_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Env_UpdateCameraLens_2.h"
int Env_UpdateCameraLens_2(int* context, void* arg2)

{
    int data_val = AI_UpdateOrders_17();
    short saved_target = data_val;
    data_val = *(arg2 + 0x1c);
    char result = TaskType_IsDerivedFrom(data_val, saved_target);

    if (result)
        return AI_MoveToNode_23(context, arg2);

    if (*(arg2 + 0x1c) == g_57c180)
        memcpy(context, arg2 + 0xf0, 0x18);

    return result;
}
