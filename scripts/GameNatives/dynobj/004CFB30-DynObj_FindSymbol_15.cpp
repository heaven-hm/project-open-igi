// IGI native provenance: DynObj_FindSymbol_15 - 0x004CFB30 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CFB30-DynObj_FindSymbol_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CFB30-DynObj_FindSymbol_15.hpp"
int DynObj_FindSymbol_15(int* context)

{
    if (context[4])
    {
        Sys_ExecuteBytecode_2(context[5]);
        context[5] = 0;
    }

    int temp_val = context[2];
    int item_8 = context[3];
    int item_4 = temp_val;

    if (*((DynObj_LookupHashItem_13() << 2) + &g_a94e84))
        (*((DynObj_LookupHashItem_13() << 2) + &g_a94e84))(&item_8);

    Resource_Flush(context[2]);
    HeapList_Remove(context);
    return Sys_ExecuteBytecode_2(context);
}
