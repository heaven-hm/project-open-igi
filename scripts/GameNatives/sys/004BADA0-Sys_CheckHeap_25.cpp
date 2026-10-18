// IGI native provenance: Sys_CheckHeap_25 - 0x004BADA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BADA0-Sys_CheckHeap_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BADA0-Sys_CheckHeap_25.hpp"
int Sys_CheckHeap_25(int* context)

{
    HeapList_Remove(context);
    return Sys_ExecuteBytecode_2(context);
}
