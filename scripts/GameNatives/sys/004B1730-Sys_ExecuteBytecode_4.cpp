// IGI native provenance: Sys_ExecuteBytecode_4 - 0x004B1730 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B1730-Sys_ExecuteBytecode_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1730-Sys_ExecuteBytecode_4.hpp"
int Sys_ExecuteBytecode_4(int* context, int arg2, int arg3)

{
    int entity_val = *context;
    return (*(entity_val * 0x8c + &g_9436c8))(entity_val, context, arg2, arg3);
}
