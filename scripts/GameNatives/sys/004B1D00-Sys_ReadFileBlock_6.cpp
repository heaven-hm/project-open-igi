// IGI native provenance: Sys_ReadFileBlock_6 - 0x004B1D00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1D00-Sys_ReadFileBlock_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B1D00-Sys_ReadFileBlock_6.hpp"
int Sys_ReadFileBlock_6(int context, int arg2)

{
    int result = context * 0x23;
    *((result << 2) + &g_9436e0) = arg2;
    return result;
}
