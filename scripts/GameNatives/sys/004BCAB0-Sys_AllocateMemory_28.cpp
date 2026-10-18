// IGI native provenance: Sys_AllocateMemory_28 - 0x004BCAB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BCAB0-Sys_AllocateMemory_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BCAB0-Sys_AllocateMemory_28.hpp"
int Sys_AllocateMemory_28(int context, int arg2, int arg3, float arg4, int arg5)

{
    int result = arg3;
    result = 0;
    Sys_UnknownDataType_2(&result, 4, arg4, arg5, 1);
    return result;
}
