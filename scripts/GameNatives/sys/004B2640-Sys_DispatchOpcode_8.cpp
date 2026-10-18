// IGI native provenance: Sys_DispatchOpcode_8 - 0x004B2640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2640-Sys_DispatchOpcode_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2640-Sys_DispatchOpcode_8.hpp"
int __fastcall Sys_DispatchOpcode_8(int context)

{
    int result = context;
    int entity_val = g_546fbc;
    result = 0;
    int entity_1 = *((entity_val << 2) + &g_a94e84);

    if (!entity_1)
        return 0;

    entity_1(&result);
    return result;
}
