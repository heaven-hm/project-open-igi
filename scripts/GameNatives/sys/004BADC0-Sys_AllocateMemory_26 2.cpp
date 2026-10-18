// IGI native provenance: Sys_AllocateMemory_26 - 0x004BADC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BADC0-Sys_AllocateMemory_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_AllocateMemory_26.h"
int* Sys_AllocateMemory_26(void* context, int arg2)

{
    int* result = Mem_Alloc(0x10, 4);
    result[2] = arg2;
    result[1] = 0;
    *result = 0;
    HeapList_Insert(context + 0x44, result);
    return result;
}
