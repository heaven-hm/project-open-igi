// IGI native provenance: ProximityMine_Process - 0x00475900 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00475900-ProximityMine_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ProximityMine_Process.h"
int ProximityMine_Process(long double context , long double arg2 , long double arg3 , long double arg4 , int* arg5 )

{
    float source_val = arg5[2];
    float* context_val = *arg5;
    Profile_ValueSet(arg5[1], source_val);
    *context_val = context;
    Profile_ValueSet(arg5[1], source_val + 1);
    context_val[1] = arg2;
    Profile_ValueSet(arg5[1], source_val + 2);
    context_val[2] = arg3;
    int result = Profile_ValueSet(arg5[1], source_val + 3);
    context_val[3] = arg4;
    arg5[2] = source_val + 4;
    return result;
}
