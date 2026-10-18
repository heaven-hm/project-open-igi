// IGI native provenance: Sys_FreeMemory_2 - 0x004B0B00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0B00-Sys_FreeMemory_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_FreeMemory_2.h"
int Sys_FreeMemory_2(int* context, int arg2, int arg3, int arg4)

{
    int data_val = arg4;

    if (data_val < 4)
        data_val = 4;

    context[5] = arg2;
    int dest_1 = (arg2 + data_val + 3) & ~(data_val - 1);
    void* data_1 = Mem_Alloc((dest_1 + 4) * arg3, data_val);
    *context = data_1;
    int result = 0;
    context[1] = dest_1 * arg3 + data_1;
    context[3] = arg3;
    context[4] = dest_1;
    context[2] = 0;

    if (arg3 > 0)
    {
        do
        {
            *(context[1] + (result << 2)) = result;
            result += 1;
        } while (result < arg3);
    }

    return result;
}
