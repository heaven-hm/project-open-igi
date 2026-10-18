// IGI native provenance: Display_CreateSurface_11 - 0x00496D80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496D80-Display_CreateSurface_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_11.h"
int Display_CreateSurface_11(int context, void* arg2, int arg3, int arg4)

{
    if (arg3 <= 0)
        return arg3;

    Resource_Unpack(*(arg2 + 0x18), context, arg3 * arg4);
    return Sys_ResolveSymbol_4(*(arg2 + 0x18));
}
