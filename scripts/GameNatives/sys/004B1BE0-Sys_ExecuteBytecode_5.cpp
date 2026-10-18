// IGI native provenance: Sys_ExecuteBytecode_5 - 0x004B1BE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1BE0-Sys_ExecuteBytecode_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1BE0-Sys_ExecuteBytecode_5.hpp"
int Sys_ExecuteBytecode_5(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436b0) = arg2;
    return result;
}
