// IGI native provenance: Sys_ResolveSymbol_5 - 0x004B1C60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1C60-Sys_ResolveSymbol_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ResolveSymbol_5.h"
int Sys_ResolveSymbol_5(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436c0) = arg2;
    return result;
}
