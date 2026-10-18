// IGI native provenance: Sys_OpenFileStream_8 - 0x004B2450 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B2450-Sys_OpenFileStream_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2450-Sys_OpenFileStream_8.hpp"
int Sys_OpenFileStream_8(int context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)

{
    int item_30 = context;
    int item_14 = arg5;
    int item_18 = arg4;
    int item_4 = arg7;
    int item_10 = arg6;
    int item_c = 0;
    int item_8 = g_943b3c;
    return (*((g_546fb0 << 2) + &g_a94e84))(&item_30, item_30, arg2, 0, arg3);
}
