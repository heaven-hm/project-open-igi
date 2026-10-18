// IGI native provenance: HeapList_Remove - 0x004AF960 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF960-HeapList_Remove.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HeapList_Remove.h"
int* HeapList_Remove(int* context)

{
    *context[1] = *context;
    *(*context + 4) = context[1];
    context[1] = 0;
    *context = 0;
    return context;
}
