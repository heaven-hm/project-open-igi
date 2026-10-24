// IGI native provenance: QTaskList_Register - 0x004C1790 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C1790-DynObj_QtasklistIsFull.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_QtasklistIsFull.h"
int* QTaskList_Register(int* context, int arg2)

{
    int entity_val = *context;

    if (entity_val < context[1])
    {
        context[entity_val + 2] = arg2;
        *context += 1;
        return context;
    }

    Error_Show("QTaskList is full");

    while (true)
        /* nop */
}
