// IGI native provenance: Sys_ExecuteBytecode_3 - 0x004B0FD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0FD0-Sys_ExecuteBytecode_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0FD0-Sys_ExecuteBytecode_3.hpp"
int Sys_ExecuteBytecode_3(int context)

{
    int entity_val = g_a94a64;
    int data_val = g_942304 - 1;
    g_942304 = data_val;
    *((data_val << 2) + &g_942308) = entity_val;
    return Sys_FreeMemory_3(context);
}
