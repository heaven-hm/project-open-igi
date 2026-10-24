// IGI native provenance: CameraBase_Cleanup_8 - 0x004F03A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F03A0-CameraBase_Cleanup_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F03A0-CameraBase_Cleanup_8.hpp"
int CameraBase_Cleanup_8(char* context, int* arg2)

{
    Script_TooFewParametersInLi(arg2, 1);
    Sys_ResolveSymbol_23(arg2[1], *arg2, context, 0x20);
    int result = *arg2 + 1;
    *arg2 = result;
    return result;
}
