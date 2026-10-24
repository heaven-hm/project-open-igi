// IGI native provenance: CameraBase_Cleanup_6 - 0x004F0350 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0350-CameraBase_Cleanup_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Cleanup_6.h"
int CameraBase_Cleanup_6(char* context, int* arg2)

{
    Script_TooFewParametersInLi(arg2, 1);
    Sys_ResolveSymbol_23(arg2[1], *arg2, context, 0x10);
    int result = *arg2 + 1;
    *arg2 = result;
    return result;
}
