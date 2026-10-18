// IGI native provenance: Sys_CheckHeap_30 - 0x004BF1E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BF1E0-Sys_CheckHeap_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BF1E0-Sys_CheckHeap_30.hpp"
int Sys_CheckHeap_30(int context)

{
    return (*(context + 0x18))(context);
}
