// IGI native provenance: Heap_FreeEntry - 0x004B7030 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7030-Sys_PushStack_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_PushStack_18.h"
int Heap_FreeEntry(int* context)

{
    Sys_ReadFileBlock_17(context);
    return Sys_ExecuteBytecode_2(context);
}
