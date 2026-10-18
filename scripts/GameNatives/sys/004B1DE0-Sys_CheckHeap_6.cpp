// IGI native provenance: Sys_CheckHeap_6 - 0x004B1DE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1DE0-Sys_CheckHeap_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1DE0-Sys_CheckHeap_6.hpp"
void* Sys_CheckHeap_6(int context) {
    return context * 0x8c + &g_94372c;
}
