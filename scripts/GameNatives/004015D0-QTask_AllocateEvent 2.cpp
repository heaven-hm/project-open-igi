// IGI native provenance: QTask_AllocateEvent - 0x004015D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004015D0-QTask_AllocateEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_AllocateEvent.h"
int* QTask_AllocateEvent(int* context)

{
    int* result = context;

    if (result[5])
        result = HeapList_Remove(result);

    return result;
}
