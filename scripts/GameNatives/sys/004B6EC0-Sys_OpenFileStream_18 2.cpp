// IGI native provenance: Sys_OpenFileStream_18 - 0x004B6EC0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6EC0-Sys_OpenFileStream_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream_18.h"
int** Sys_OpenFileStream_18(void* context, int arg2, int arg3, char* arg4)

{
    int** result = Mem_Alloc(0x28, 4);
    memset(result, 0, 0x28);
    *result = Sys_DispatchOpcode_20(0x100, 1, context);
    result[5] = 0x43000000;
    result[6] = 0x43000000;
    result[7] = 0x43000000;
    result[8] = 0;
    result[9] = 0;
    result[4] = 0x3f800000;
    result[1] = arg2;
    result[2] = arg3;
    Sys_FreeMemory_18(result, arg4);
    return result;
}
