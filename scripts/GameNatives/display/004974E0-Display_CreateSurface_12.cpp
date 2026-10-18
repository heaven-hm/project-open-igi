// IGI native provenance: Display_CreateSurface_12 - 0x004974E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004974E0-Display_CreateSurface_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_12.hpp"
int* Display_CreateSurface_12(int* context)

{
    context[4] = 0;
    context[6] = 0;
    *context = 0x3f800000;
    context[1] = 0x3f800000;
    context[2] = 0;
    context[5] = 0;
    context[7] = 0x3ff00000;
    return context;
}
