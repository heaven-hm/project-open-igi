// IGI native provenance: Symbol_Remove - 0x004C0460 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C0460-Symbol_Remove.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C0460-Symbol_Remove.hpp"
int Symbol_Remove(int* context, int* arg2)

// Sig: int Symbol_Remove(int* symTable, char* symName)
// Note: Removes a symbol from the game-data symbol hash table; warns "Unable to
// remove symbol \"%s\" (not registered)" when absent. Params: symTable = symbol
// table, symName = symbol name string. RETURNS non-zero on success. Call after
// dynamically registered symbols are no longer needed.
{
    char* source_val = arg2;

    if (DynObj_InsertHashItem_2(context, &arg2, source_val))
    {
        HeapList_Remove(arg2);
        return Sys_ExecuteBytecode_2(arg2);
    }

    char* item_8_2 = source_val;
    Error_Show("Unable to remove symbol "%s" (not registered)");

    while (true)
        /* nop */
}
