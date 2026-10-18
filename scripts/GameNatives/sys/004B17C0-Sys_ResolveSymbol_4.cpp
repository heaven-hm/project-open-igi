// IGI native provenance: Sys_ResolveSymbol_4 - 0x004B17C0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B17C0-Sys_ResolveSymbol_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B17C0-Sys_ResolveSymbol_4.hpp"
int Sys_ResolveSymbol_4(int* context)

{
    int entity_val = *context;
    return (*(entity_val * 0x8c + &g_9436e4))(entity_val, context, 0);
}
