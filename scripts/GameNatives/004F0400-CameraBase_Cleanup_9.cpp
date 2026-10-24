// IGI native provenance: CameraBase_Cleanup_9 - 0x004F0400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0400-CameraBase_Cleanup_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F0400-CameraBase_Cleanup_9.hpp"
int CameraBase_Cleanup_9(char* context, int* arg2)

{
    Script_TooFewParametersInLi(arg2, 1);
    Sys_ResolveSymbol_23(arg2[1], *arg2, context, 0x100);
    int result = *arg2 + 1;
    *arg2 = result;
    return result;
}
