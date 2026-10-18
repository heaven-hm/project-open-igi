// IGI native provenance: WeatherEnv_InvokeGetArg - 0x00436740 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00436740-WeatherEnv_InvokeGetArg.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00436740-WeatherEnv_InvokeGetArg.hpp"
int WeatherEnv_InvokeGetArg(long double arg1, int* arg2)

{
    int dest_ptr = arg2[2];
    int* base_val = *arg2;
    *base_val = Script_GetArg(arg2[1], dest_ptr);
    int result = Profile_ValueSet(arg2[1], dest_ptr + 1);
    base_val[1] = arg1;
    return result;
}
