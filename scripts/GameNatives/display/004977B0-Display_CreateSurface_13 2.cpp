// IGI native provenance: Display_CreateSurface_13 - 0x004977B0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004977B0-Display_CreateSurface_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_13.h"
int** Display_CreateSurface_13(int context, int arg2)

{
    int* data_val = Display_CloseDevice_14(0x24);
    memcpy(data_val, context, 0x24);
    data_val[1] = Display_SetGammaRamp_4();
    return Display_ClearSurfaces_14(arg2, data_val);
}
