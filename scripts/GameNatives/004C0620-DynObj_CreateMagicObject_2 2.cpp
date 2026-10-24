// IGI native provenance: DynObj_CreateMagicObject_2 - 0x004C0620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0620-DynObj_CreateMagicObject_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_CreateMagicObject_2.h"
int DynObj_CreateMagicObject_2(int* context, int* arg2, char* arg3)

{
    if (!DynObj_InsertHashItem_2(context, &arg3, arg3))
    {
        if (arg2)
            *arg2 = 0;

        return 0;
    }

    char* entity_val = arg3;

    if (arg2)
        *arg2 = *(entity_val + 0xc);

    return *(entity_val + 8);
}
