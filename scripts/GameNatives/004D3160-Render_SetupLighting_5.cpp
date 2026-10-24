// IGI native provenance: Render_SetupLighting_5 - 0x004D3160 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D3160-Render_SetupLighting_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D3160-Render_SetupLighting_5.hpp"
float* Render_SetupLighting_5(float* context, float* arg2, float* arg3, float arg4)

{
    float item_8_1 = (arg2[1] - arg3[1]) * arg4;
    float item_4_1 = (arg2[2] - arg3[2]) * arg4;
    *context = (*arg2 - *arg3) * arg4 + *arg3;
    context[1] = item_8_1 + arg3[1];
    context[2] = item_4_1 + arg3[2];
    return arg3;
}
