// IGI native provenance: Sys_ReadFileBlock_23 - 0x004B88F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B88F0-Sys_ReadFileBlock_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B88F0-Sys_ReadFileBlock_23.hpp"
void* Sys_ReadFileBlock_23(int* context, int arg2, int arg3, int arg4)

{
    return Q_TaskHashTableSet(context, arg2, arg3, arg4, 1, 2, 0);
}
