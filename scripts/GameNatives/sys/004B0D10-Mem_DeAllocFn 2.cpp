// IGI native provenance: Mem_DeAllocFn - 0x004B0D10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0D10-Sys_ExecuteBytecode_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ExecuteBytecode_2.h"
int Mem_DeAllocFn(void* context)

{
    int* data_val = context - 4;
    data_val &= 0xfc;
    g_942300 -= 1;
    int* source_val = *data_val;
    int* item_c = source_val;
    return Camera_UpdateTransform_9(HeapList_Remove(source_val));
}
