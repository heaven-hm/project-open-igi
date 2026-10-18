// IGI native provenance: CameraBase_Cleanup_3 - 0x004F00B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F00B0-CameraBase_Cleanup_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Cleanup_3.h"
int CameraBase_Cleanup_3(int* context, int* arg2)

{
    Script_TooFewParametersInLi(arg2, 1);
    *context = Script_GetArg(arg2[1], *arg2);
    int result = *arg2 + 1;
    *arg2 = result;
    return result;
}
