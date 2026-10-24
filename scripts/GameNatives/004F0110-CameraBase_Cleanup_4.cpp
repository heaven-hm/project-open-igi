// IGI native provenance: CameraBase_Cleanup_4 - 0x004F0110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0110-CameraBase_Cleanup_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F0110-CameraBase_Cleanup_4.hpp"
int CameraBase_Cleanup_4(short* context, int* arg2)

{
    Script_TooFewParametersInLi(arg2, 1);
    *context = Script_GetArg(arg2[1], *arg2);
    int result = *arg2 + 1;
    *arg2 = result;
    return result;
}
