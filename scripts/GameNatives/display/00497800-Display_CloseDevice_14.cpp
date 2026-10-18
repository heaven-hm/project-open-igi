// IGI native provenance: Display_CloseDevice_14 - 0x00497800 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00497800-Display_CloseDevice_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CloseDevice_14.hpp"
int Display_CloseDevice_14(int context)

{
    int result = g_6e5b98;
    int entity_val = result + context;
    int temp_1 = g_6e5ba0;
    g_6e5b98 = entity_val;

    if (entity_val - 0x684118 > temp_1)
        g_6e5ba0 = entity_val - 0x684118;

    return result;
}
