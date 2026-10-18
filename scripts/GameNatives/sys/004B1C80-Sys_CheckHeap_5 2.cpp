// IGI native provenance: Sys_CheckHeap_5 - 0x004B1C80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1C80-Sys_CheckHeap_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_CheckHeap_5.h"
int Sys_CheckHeap_5(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436c4) = arg2;
    return result;
}
