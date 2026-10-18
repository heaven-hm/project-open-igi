// IGI native provenance: Sys_OpenFileStream_17 - 0x004B65E0 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B65E0-Sys_OpenFileStream_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B65E0-Sys_OpenFileStream_17.hpp"
int Sys_OpenFileStream_17(int* context, char arg2, short arg3, short arg4, short arg5, short arg6, short arg7, int arg8, int arg9, int arg10, int arg11)

{
    int context_1 = (arg10 * 9 + 9) << 2;
    *context = arg9;
    context[5] = arg10;
    context[3] = 0;
    *(context + 0x16) = arg2;
    void* data_1 = Mem_Alloc(context_1 + arg11 * 0x24, 4);
    context[4] = arg11;
    context[2] = data_1;
    context[1] = data_1 + context_1;
    memset(memset(data_1, 0, context_1 & 0xfffffffc), 0, context_1 & 3);
    int result = arg10 * 0x24;
    *(context[2] + result + 4) = arg8;
    *(context[2] + result + 8) = arg3;
    *(context[2] + result + 0xa) = arg4;
    *(context[2] + result + 0xc) = arg5;
    *(context[2] + result + 0xe) = arg6;
    *(context[2] + result + 0x10) = arg7;
    return result;
}
