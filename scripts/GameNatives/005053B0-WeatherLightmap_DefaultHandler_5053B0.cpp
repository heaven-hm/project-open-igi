// IGI native provenance: WeatherLightmap_DefaultHandler_5053B0 - 0x005053B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005053B0-WeatherLightmap_DefaultHandler_5053B0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "005053B0-WeatherLightmap_DefaultHandler_5053B0.hpp"
int* WeatherLightmap_DefaultHandler_5053B0(void* arg1, int* arg2)

{
    *(arg1 + 0x38) = *(arg1 + 0x2c);
    *(arg1 + 0x3c) = *(arg1 + 0x30);
    *(arg1 + 0x40) = *(arg1 + 0x34);
    *(arg1 + 0x2c) = *arg2;
    *(arg1 + 0x30) = arg2[1];
    *(arg1 + 0x34) = arg2[2];
    return arg1 + 0x2c;
}
