// IGI native provenance: GunM16A2_Process - 0x00473790 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00473790-GunM16A2_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunM16A2_Process.h"
int GunM16A2_Process(long double context , long double arg2 , long double arg3 , int* arg4 )

{
    float source_val = arg4[2];
    float* context_val = *arg4;
    Profile_ValueSet(arg4[1], source_val);
    *context_val = context;
    Profile_ValueSet(arg4[1], source_val + 1);
    context_val[1] = arg2;
    int result = Profile_ValueSet(arg4[1], source_val + 2);
    context_val[2] = arg3;
    arg4[2] = source_val + 3;
    return result;
}
