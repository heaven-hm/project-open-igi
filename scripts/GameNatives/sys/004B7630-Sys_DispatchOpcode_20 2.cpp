// IGI native provenance: Sys_DispatchOpcode_20 - 0x004B7630 - CFG score 80.9% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B7630-Sys_DispatchOpcode_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_DispatchOpcode_20.h"
int* Sys_DispatchOpcode_20(int context, int arg2, void* arg3)

{
    int flagP1 = context * arg2;
    int* result = Mem_Alloc(flagP1 + 0x13, 4);
    memset(memset(result, 0, (flagP1 + 0x13) & 0xfffffffc), 0,
        (flagP1 + 0x13) & 3);
    int data_val = Sys_PushStack_17(arg3);
    int data_1 = -(data_val);
    *result = data_1 - data_1 + 1;
    result[1] = context;
    *(result + 6) = arg2;
    result[2] = arg3;
    result[3] = Mem_Alloc(arg2 * 2, 4);
    int data_6 = 0;

    if (arg2 > 0)
    {
        do
        {
            data_6 += 1;
            *(result[3] + (data_6 << 1) - 2) = 0;
        } while (data_6 < arg2);
    }

    if (flagP1 > 0)
        memset(memset(&result[4], 0, flagP1 & 0xfffffffc), 0, flagP1 & 3);

    return result;
}
