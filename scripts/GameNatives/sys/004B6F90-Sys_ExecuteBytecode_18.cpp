// IGI native provenance: Sys_ExecuteBytecode_18 - 0x004B6F90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6F90-Sys_ExecuteBytecode_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B6F90-Sys_ExecuteBytecode_18.hpp"
int Sys_ExecuteBytecode_18(int context, float* arg2)

{
    int* source_val = context;
    *arg2 = source_val[1];
    arg2[1] = source_val[2];
    arg2[2] = Sys_CheckHeap_21(source_val);
    int result = Sys_AllocateMemory_22(source_val);
    arg2[3] = result;
    return result;
}
