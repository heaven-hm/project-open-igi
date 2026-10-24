// IGI native provenance: CameraBase_Update_2 - 0x004EF9E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EF9E0-CameraBase_Update_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004EF9E0-CameraBase_Update_2.hpp"
int CameraBase_Update_2(long double context , float* arg2, int* arg3)

{
    Script_TooFewParametersInLi(arg3, 1);
    Profile_ValueSet(arg3[1], *arg3);
    *arg2 = context;
    int result = *arg3 + 1;
    *arg3 = result;
    return result;
}
