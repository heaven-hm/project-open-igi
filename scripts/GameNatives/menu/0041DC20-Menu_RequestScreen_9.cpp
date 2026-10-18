// IGI native provenance: Menu_RequestScreen_9 - 0x0041DC20 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0041DC20-Menu_RequestScreen_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_RequestScreen_9.hpp"
int Menu_RequestScreen_9(int context)

{
    int dest_val = context;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int result = ~i;
    int source_1;
    int dest_3;
    dest_3 = memcpy(g_57bbcc + 0x54, dest_val - result, result & 0xfffffffc);
    memcpy(dest_3, source_1, result & 3);
    return result;
}
