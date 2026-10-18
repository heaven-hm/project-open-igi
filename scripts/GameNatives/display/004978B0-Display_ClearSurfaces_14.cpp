// IGI native provenance: Display_ClearSurfaces_14 - 0x004978B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004978B0-Display_ClearSurfaces_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_14.hpp"
int** Display_ClearSurfaces_14(int context, int* arg2)

{
    int entity_val = g_bcabe4;
    int data_val = context * 3;
    int** result = entity_val + (data_val << 2);
    *arg2 = *(entity_val + (data_val << 2));
    *result = arg2;
    return result;
}
