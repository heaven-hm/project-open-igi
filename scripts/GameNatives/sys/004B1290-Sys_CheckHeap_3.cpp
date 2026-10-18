// IGI native provenance: Sys_CheckHeap_3 - 0x004B1290 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1290-Sys_CheckHeap_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1290-Sys_CheckHeap_3.hpp"
int Sys_CheckHeap_3(int context, int arg2)

{
    int dest_val = arg2;
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
    dest_3 = memcpy(context * 0x8c + &g_94372c, dest_val - result, result & 0xfffffffc);
    memcpy(dest_3, source_1, result & 3);
    return result;
}
