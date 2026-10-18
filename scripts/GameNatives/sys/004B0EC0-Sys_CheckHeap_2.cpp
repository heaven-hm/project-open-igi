// IGI native provenance: Sys_CheckHeap_2 - 0x004B0EC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0EC0-Sys_CheckHeap_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0EC0-Sys_CheckHeap_2.hpp"
int Sys_CheckHeap_2(int context, int arg2, int arg3)

{
    int result = arg2 + context * 0x82;
    *((result << 2) + &g_a950a4) = arg3;
    return result;
}
