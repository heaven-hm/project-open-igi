// IGI native provenance: Display_SetDisplayMode_15 - 0x00497E60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00497E60-Display_SetDisplayMode_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetDisplayMode_15.hpp"
void* Display_SetDisplayMode_15(void* context, float arg2)

{
    *(context + 0x5c) = arg2;
    return context;
}
