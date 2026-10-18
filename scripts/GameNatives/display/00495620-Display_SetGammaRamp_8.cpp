// IGI native provenance: Display_SetGammaRamp_8 - 0x00495620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00495620-Display_SetGammaRamp_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_8.hpp"
int* Display_SetGammaRamp_8(int* context, int* arg2)

{
    int temp_val = *arg2;
    context[1] = 0;
    *context = temp_val;
    void* temp_1 = *arg2;

    if (temp_1)
        *(temp_1 + 4) = context;

    *arg2 = context;
    return context;
}
