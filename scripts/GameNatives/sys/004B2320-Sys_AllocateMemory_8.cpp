// IGI native provenance: Sys_AllocateMemory_8 - 0x004B2320 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2320-Sys_AllocateMemory_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2320-Sys_AllocateMemory_8.hpp"
int Sys_AllocateMemory_8(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8)

{
    *context = Sys_ReadFileBlock_3();
    int* item_30;
    memset(&item_30, 0, 0x30);
    int item_2c = arg2;
    item_30 = context;
    int item_24 = arg3;
    int item_14 = arg5;
    int item_18 = arg4;
    int item_c = arg8;
    int item_10 = arg6;
    int item_8 = g_943b3c;
    int item_4 = 0;
    int item_28 = arg7;
    return (*((g_546fa8 << 2) + &g_a94e84))(&item_30);
}
