// IGI native provenance: DynObj_InsertHashItem_12 - 0x004CE5C0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE5C0-DynObj_InsertHashItem_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CE5C0-DynObj_InsertHashItem_12.hpp"
short* DynObj_InsertHashItem_12(int context, int arg2)

{
    short* result = *(context + (arg2 << 2) + 0x100);

    if (result != g_536148)
    {
        memset(context + arg2 * 0x1fc + 0x118, 0, 0x1fc);
        result = DynObj_FindSymbol_12(context, DynObj_InsertHashItem_9(context, arg2));
        *(context + (arg2 << 2) + 0x100) = g_536148;
    }

    return result;
}
