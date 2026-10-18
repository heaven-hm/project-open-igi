// IGI native provenance: FX_TrailUpdate_26 - 0x0048F300 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0048F300-FX_TrailUpdate_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048F300-FX_TrailUpdate_26.hpp"
int FX_TrailUpdate_26(int context)

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
    int dest_2;
    dest_2 = memcpy(&g_5c8c20, dest_val - result, result & 0xfffffffc);
    memcpy(dest_2, source_1, result & 3);
    return result;
}
