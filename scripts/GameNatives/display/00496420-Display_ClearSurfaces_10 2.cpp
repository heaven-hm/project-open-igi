// IGI native provenance: Display_ClearSurfaces_10 - 0x00496420 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496420-Display_ClearSurfaces_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_10.h"
short* Display_ClearSurfaces_10(short* context, short arg2)

{
    *context = arg2;
    context[3] = 0;
    context[1] = 0;
    context[4] = 0;
    context[2] = 0;
    context[5] = 0;
    return context;
}
