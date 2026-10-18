// IGI native provenance: Display_ClearSurfaces_3 - 0x004916E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004916E0-Display_ClearSurfaces_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_3.hpp"
int* Display_ClearSurfaces_3(int context)

{
    return Display_SetDisplayMode_8(context, &g_bcae00);
}
