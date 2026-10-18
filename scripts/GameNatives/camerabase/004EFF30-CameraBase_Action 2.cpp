// IGI native provenance: CameraBase_Action - 0x004EFF30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFF30-CameraBase_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Action.h"
int CameraBase_Action(long double context , double* arg2, int* arg3)

{
    Script_TooFewParametersInLi(arg3, 1);
    Profile_ValueSet(arg3[1], *arg3);
    *arg2 = context;
    int result = *arg3 + 1;
    *arg3 = result;
    return result;
}
