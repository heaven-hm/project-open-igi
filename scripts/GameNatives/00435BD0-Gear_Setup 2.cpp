// IGI native provenance: Gear_Setup - 0x00435BD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00435BD0-Gear_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Gear_Setup.h"
int Gear_Setup(long double context , long double arg2 , long double arg3 , long double arg4 , int* arg5 )

{
    float source_val = arg5[2];
    float* context_val = *arg5;
    Profile_ValueSet(arg5[1], source_val);
    *context_val = context;
    context_val[1] = Script_GetArg(arg5[1], source_val + 1);
    Profile_ValueSet(arg5[1], source_val + 2);
    context_val[2] = arg2;
    Profile_ValueSet(arg5[1], source_val + 3);
    context_val[3] = arg3;
    int result = Profile_ValueSet(arg5[1], source_val + 4);
    context_val[4] = arg4;
    return result;
}
