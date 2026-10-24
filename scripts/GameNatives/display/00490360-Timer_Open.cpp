// IGI native provenance: Timer_Open - 0x00490360 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00490360-Display_ClearSurfaces.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00490360-Timer_Open.hpp"
unsigned int Timer_Open()

{
    unsigned int result = GetTickCount();
    g_tick_base = result;
    return result;
}
