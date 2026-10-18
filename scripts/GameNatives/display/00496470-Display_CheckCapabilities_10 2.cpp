// IGI native provenance: Display_CheckCapabilities_10 - 0x00496470 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496470-Display_CheckCapabilities_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CheckCapabilities_10.h"
int* Display_CheckCapabilities_10(int context, int arg2, int arg3)

{
    int* result = *(context + arg2 * 0xc);

    for (int* i = *result; i; i = *i)
    {
        if (result[3] == arg3)
            return result;

        result = i;
    }

    return nullptr;
}
