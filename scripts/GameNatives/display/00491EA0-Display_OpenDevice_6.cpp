// IGI native provenance: Display_OpenDevice_6 - 0x00491EA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491EA0-Display_OpenDevice_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_OpenDevice_6.hpp"
int Display_OpenDevice_6(int context, int arg2)

{
    int item_c = context;
    int result;
    int* item_8 = &result;
    int item_4 = arg2;
    int entity_2 = *((g_542334 << 2) + &g_a94e84);
    result = 0;

    if (!entity_2)
        return 0;

    entity_2(&item_c);
    return result;
}
