// IGI native provenance: Sys_ResolveSymbol_2 - 0x004B0EB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0EB0-Sys_ResolveSymbol_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0EB0-Sys_ResolveSymbol_2.hpp"
int Sys_ResolveSymbol_2(int context)

{
    *((context << 3) + &g_a94a84) = 0;
    return context;
}
