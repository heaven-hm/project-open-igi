// IGI native provenance: Sys_PushStack_8 - 0x004B2670 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2670-Sys_PushStack_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2670-Sys_PushStack_8.hpp"
int __fastcall Sys_PushStack_8(int context)

{
    int result = context;
    int entity_val = g_546fb4;
    result = 1;
    int entity_1 = *((entity_val << 2) + &g_a94e84);

    if (!entity_1)
        return 1;

    entity_1(&result);
    return result;
}
