// IGI native provenance: CameraBase_OnEvent - 0x004EFBA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EFBA0-CameraBase_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004EFBA0-CameraBase_OnEvent.hpp"
int CameraBase_OnEvent(long double context , long double arg2 , long double arg3 , float* arg4, float arg5)

{
    int* source_val = arg5;
    float dest_val = *source_val;
    Script_TooFewParametersInLi(source_val, 3);
    Profile_ValueSet(source_val[1], dest_val);
    Profile_ValueSet(source_val[1], dest_val + 1);
    Profile_ValueSet(source_val[1], dest_val + 2);
    Sys_ReadFileBlock_12(arg4, context, arg2, arg3);
    int result = *source_val + 3;
    *source_val = result;
    return result;
}
