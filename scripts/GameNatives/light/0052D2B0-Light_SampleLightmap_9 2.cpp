// IGI native provenance: Light_SampleLightmap_9 - 0x0052D2B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D2B0-Light_SampleLightmap_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_SampleLightmap_9.h"
int Light_SampleLightmap_9(int* context, int* arg2, int* arg3, int* arg4, int* arg5, int* arg6)

{
    *context = *arg2;
    context[1] = arg2[1];
    context[2] = arg2[2];
    context[3] = *arg2;
    context[4] = arg2[1];
    context[5] = arg2[2];
    context[6] = *arg3;
    context[7] = arg3[1];
    context[8] = arg3[2];
    context[9] = *arg4;
    context[0xa] = arg4[1];
    context[0xb] = arg4[2];
    context[0xc] = *arg5;
    context[0xd] = arg5[1];
    context[0xe] = arg5[2];
    context[0xf] = *arg6;
    context[0x10] = arg6[1];
    context[0x11] = arg6[2];
    return Light_ComputeDirectional(context);
}
