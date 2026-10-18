// IGI native provenance: DynObj_InsertHashItem_15 - 0x004CFBB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CFBB0-DynObj_InsertHashItem_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CFBB0-DynObj_InsertHashItem_15.hpp"
int DynObj_InsertHashItem_15(int* context)

{
    Resource_Flush(context[4]);
    HeapList_Remove(context);
    return Sys_ExecuteBytecode_2(context);
}
