// IGI native provenance: Sys_ResolveSymbol_16 - 0x004B64B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B64B0-Sys_ResolveSymbol_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B64B0-Sys_ResolveSymbol_16.hpp"
int Sys_ResolveSymbol_16(int* context)

{
    int result = Is_ResourceLoaded(context, &context);

    if (result)
        result = Resource_FreeNode(context);

    return result;
}
