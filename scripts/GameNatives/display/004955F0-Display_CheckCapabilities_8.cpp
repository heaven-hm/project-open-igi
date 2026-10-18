// IGI native provenance: Display_CheckCapabilities_8 - 0x004955F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004955F0-Display_CheckCapabilities_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CheckCapabilities_8.hpp"
int* Display_CheckCapabilities_8(void* context)

{
    int i_1 = 0x65;
    int* result = context + 4;
    int i;

    do
    {
        result[-1] = result;
        result[1] = &result[-1];
        *result = 0;
        result = &result[3];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    return result;
}
