// IGI native provenance: Display_SetGammaRamp_7 - 0x004950F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004950F0-Display_SetGammaRamp_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_7.h"
int* Display_SetGammaRamp_7(int* context, int* arg2, int* arg3)

{
    *context = *arg2;
    context[1] = arg2[3];
    context[2] = arg2[6];
    context[3] = 0;
    context[4] = arg2[1];
    context[5] = arg2[4];
    context[6] = arg2[7];
    context[7] = 0;
    context[8] = arg2[2];
    context[9] = arg2[5];
    context[0xa] = arg2[8];
    context[0xb] = 0;

    if (!arg3)
    {
        context[0xc] = 0;
        context[0xd] = 0;
        context[0xe] = 0;
        context[0xf] = 0x3f800000;
        return context;
    }

    context[0xc] = *arg3;
    context[0xd] = arg3[1];
    context[0xe] = arg3[2];
    context[0xf] = 0x3f800000;
    return context;
}
