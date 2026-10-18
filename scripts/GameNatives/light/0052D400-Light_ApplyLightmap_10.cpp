// IGI native provenance: Light_ApplyLightmap_10 - 0x0052D400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D400-Light_ApplyLightmap_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_ApplyLightmap_10.hpp"
int Light_ApplyLightmap_10(int* context)

{
    g_54f720 = *context;
    g_54f724 = context[1];
    int result = context[2];
    g_54f728 = result;
    return result;
}
