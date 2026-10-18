// IGI native provenance: Display_CloseDevice_13 - 0x00497520 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00497520-Display_CloseDevice_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CloseDevice_13.hpp"
int Display_CloseDevice_13(void* context, int* arg2, int* arg3)

{
    *(context + 0x30) = *arg2;
    *(context + 0x34) = arg2[1];
    *(context + 0x38) = *arg3;
    *(context + 0x3c) = arg3[1];
    int result = g_684110 + 1;
    g_684110 = result;
    return result;
}
