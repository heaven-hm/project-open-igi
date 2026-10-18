// IGI native provenance: Sys_AllocateMemory_21 - 0x004B77F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B77F0-Sys_AllocateMemory_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B77F0-Sys_AllocateMemory_21.hpp"
void* Sys_AllocateMemory_21(int* context, char* arg2, int arg3, void* arg4)

{
    char* source_val = arg2;
    char i = *source_val;
    int temp_2 = *(context[2] + 0xc) + 0x10;

    if (i)
    {
        void* data_3 = context + context[1] * arg4 + arg3 + 0x10;

        do
        {
            data_3 += 1;
            source_val = &source_val[1];
            *(data_3 - 1) = *(i + temp_2);
            i = *source_val;
        } while (i);
    }

    return Sys_CheckHeap_20(context, arg4);
}
