// IGI native provenance: DynObj_LookupHashItem_7 - 0x004C5C90 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004C5C90-DynObj_LookupHashItem_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_LookupHashItem_7.h"
int* DynObj_LookupHashItem_7(int* context , int arg2, int arg3, int arg4)

{
    context[0xe] = 0x8000;
    *(context + 0x3a) = 0x8000;
    context[0xc] = 0;
    context[1] = arg2;
    *context = arg3;
    memset(&context[2], 0, 0x28);
    context[0xd] = arg4;
    context[0xf] = 0;
    return context;
}
