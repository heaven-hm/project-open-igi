// IGI native provenance: Display_CloseDevice_12 - 0x00496E00 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00496E00-Display_CloseDevice_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CloseDevice_12.hpp"
int Display_CloseDevice_12(int context, int arg2, int arg3)

{
    memcpy(&g_5ca160, context, 0x28);
    memcpy(&g_5ca138, arg2, 0x18);
    memcpy(0x676ab8, arg3, 0x18);
}
