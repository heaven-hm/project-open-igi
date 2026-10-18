// IGI native provenance: DynObj_RegisterSymbol_4 - 0x004C1530 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C1530-DynObj_RegisterSymbol_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C1530-DynObj_RegisterSymbol_4.hpp"
int DynObj_RegisterSymbol_4(void* context, int arg2)

{
    if (arg2 < *(context + 8))
    {
        int entity_1 = *(*(context + 0x10) + (arg2 << 2));

        if (entity_1 < *(context + 0xc))
            return *(context + 0x14) + entity_1;
    }

    return 0;
}
