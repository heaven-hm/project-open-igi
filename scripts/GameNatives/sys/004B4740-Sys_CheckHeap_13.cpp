// IGI native provenance: Sys_CheckHeap_13 - 0x004B4740 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B4740-Sys_CheckHeap_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B4740-Sys_CheckHeap_13.hpp"
int Sys_CheckHeap_13(int* context)

{
    int temp_1 = context[2] ^ *context >> 7;
    context[2] = temp_1;
    int temp_3 = context[1] ^ temp_1;
    context[1] = temp_3;
    int result = *context + temp_3;
    *context = result;
    return result;
}
