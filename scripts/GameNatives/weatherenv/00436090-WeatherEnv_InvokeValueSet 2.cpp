// IGI native provenance: WeatherEnv_InvokeValueSet - 0x00436090 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00436090-WeatherEnv_InvokeValueSet.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "WeatherEnv_InvokeValueSet.h"

int WeatherEnv_InvokeValueSet(long double arg1, int* arg2)

{
    float* source_ptr = *arg2;
    int result = Profile_ValueSet(arg2[1], arg2[2]);
    *source_ptr = arg1;
    return result;
}
