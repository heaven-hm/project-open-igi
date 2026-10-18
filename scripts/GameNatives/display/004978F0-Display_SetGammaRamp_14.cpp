// IGI native provenance: Display_SetGammaRamp_14 - 0x004978F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004978F0-Display_SetGammaRamp_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetGammaRamp_14.hpp"
int Display_SetGammaRamp_14(int context, int arg2)

{
    int item_8 = context;
    int item_4 = arg2;
    return Sys_AllocateMemory_3(sub_497920, &item_8);
}
