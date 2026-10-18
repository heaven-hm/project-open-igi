// IGI native provenance: Light_SampleLightmap_10 - 0x0052D5B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D5B0-Light_SampleLightmap_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_SampleLightmap_10.h"
int Light_SampleLightmap_10(int* context, char* arg2)

{
    int item_4 = context[2];
    int item_8 = context[1];
    int item_c = *context;
    return Game_DataSymbolLoad(arg2, "%04d-%02d-%02d");
}
