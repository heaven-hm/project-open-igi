// IGI native provenance: Display_SetGammaRamp_2 - 0x00491240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491240-Display_SetGammaRamp_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_2.h"
void Display_SetGammaRamp_2(int context, int arg2, int* arg3)

{
    if (arg2 >= 0 && arg2 < 4 && arg3 && context && !*(context + (arg2 << 2) + 0x1b8))
        *(context + (arg2 << 2) + 0x1b8) = Sys_DispatchOpcode_18(arg3);
}
