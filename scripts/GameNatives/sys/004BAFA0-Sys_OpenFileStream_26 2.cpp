// IGI native provenance: Sys_OpenFileStream_26 - 0x004BAFA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BAFA0-Sys_OpenFileStream_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream_26.h"
int Sys_OpenFileStream_26(int context, int arg2, int arg3, void* arg4, char* arg5)

{
    int result = arg3;
    int* data_1 = *(arg4 + 0x1c);
    result = 0;
    DynObj_CreateMagicObject_2(data_1, &result, arg5);
    return result;
}
