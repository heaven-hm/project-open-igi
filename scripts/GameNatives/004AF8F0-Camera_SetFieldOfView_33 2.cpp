// IGI native provenance: HeapList_Insert - 0x004AF8F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF8F0-HeapList_Insert.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HeapList_Insert.h"
void* HeapList_Insert(void* context, void** arg2)

{
    arg2[1] = *(context + 8);
    *arg2 = context + 4;
    **(context + 8) = arg2;
    *(context + 8) = arg2;
    return context;
}
