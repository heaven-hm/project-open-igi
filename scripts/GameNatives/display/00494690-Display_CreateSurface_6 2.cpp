// IGI native provenance: Display_CreateSurface_6 - 0x00494690 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00494690-Display_CreateSurface_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_6.h"
int __stdcall Display_CreateSurface_6(void* context, void* arg2)

{
    int data_val = *(context + 0xc);

    if (*(arg2 + 0xc) < data_val && *(context + 4) == 0x400 && data_val <= *(arg2 + 0x20))
        memcpy(arg2, context, 0x20);

    return 1;
}
