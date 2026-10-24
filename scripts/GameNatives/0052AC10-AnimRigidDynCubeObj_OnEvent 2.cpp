// IGI native provenance: AnimRigidDynCubeObj_OnEvent - 0x0052AC10 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0052AC10-AnimRigidDynCubeObj_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AnimRigidDynCubeObj_OnEvent.h"
int AnimRigidDynCubeObj_OnEvent(int context, int* arg2)

{
    int result = *arg2;
    memcpy(arg2[2], context + 0x20, 0x18);
    memcpy(result + 0x18, context + 0x70, 0x28);
    memcpy(result, context + 0x20, 0x18);
    return result;
}
