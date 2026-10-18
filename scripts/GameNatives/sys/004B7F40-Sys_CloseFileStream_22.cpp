// IGI native provenance: Sys_CloseFileStream_22 - 0x004B7F40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7F40-Sys_CloseFileStream_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7F40-Sys_CloseFileStream_22.hpp"
void* Sys_CloseFileStream_22(int* context, int arg2)

{
    return Q_TaskHashTableSet(context, Sys_ExecuteBytecode_22, arg2, 2, 0, 1, 1);
}
