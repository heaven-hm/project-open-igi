// IGI native provenance: Sys_ResolveSymbol_20 - 0x004B7710 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7710-Sys_ResolveSymbol_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ResolveSymbol_20.h"
void* Sys_ResolveSymbol_20(void* context, int arg2, int arg3)

{
    void* result = context;
    result = *(*(result + 4) * arg3 + result + arg2 + 0x10);
    return result;
}
