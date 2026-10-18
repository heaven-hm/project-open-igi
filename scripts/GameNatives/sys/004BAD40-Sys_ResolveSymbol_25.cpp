// IGI native provenance: Sys_ResolveSymbol_25 - 0x004BAD40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BAD40-Sys_ResolveSymbol_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BAD40-Sys_ResolveSymbol_25.hpp"
int Sys_ResolveSymbol_25()

{
    int* data_val = g_a43e88;

    if (data_val)
    {
        DynObj_RegisterSymbol_2(data_val);
        g_a43e88 = 0;
    }

    int* i_1 = g_a43ea0;

    for (int* i = *i_1; i; i = *i)
    {
        Sys_CheckHeap_25(i_1);
        i_1 = i;
    }

    int result = g_a43e8c;

    if (!result)
        return result;

    int saved_source_2 = result;
    return Warning_Show("%d script symbol references not deallocated.");
}
