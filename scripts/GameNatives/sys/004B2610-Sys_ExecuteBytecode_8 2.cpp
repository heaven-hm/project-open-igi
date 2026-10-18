// IGI native provenance: Sys_ExecuteBytecode_8 - 0x004B2610 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2610-Sys_ExecuteBytecode_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ExecuteBytecode_8.h"
int Sys_ExecuteBytecode_8(int context)

{
    int entity_val = g_546fb8;
    context = context;
    int result = *((entity_val << 2) + &g_a94e84);

    if (result)
        result = result(&context);

    return result;
}
