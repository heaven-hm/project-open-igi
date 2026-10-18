// IGI native provenance: Sys_ReadFileBlock - 0x004B0720 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0720-Sys_ReadFileBlock.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0720-Sys_ReadFileBlock.hpp"
int Sys_ReadFileBlock(int context, int* arg2, int* arg3, int* arg4, int* arg5, int* arg6)

{
    int entity_val = *arg2;
    int result;
    int context_val;

    if (entity_val <= 0)
    {
        context_val = 0xffffffff;
        result = 0;
    }
    else
    {
        entity_val -= 1;
        int temp_1 = entity_val * 5;
        context_val = *(context + (temp_1 << 2) + 0xc);
        int source_1 = *(context + (temp_1 << 2) + 8);
        int dest_1 = *(context + (temp_1 << 2) + 4);
        int flagP1 = *(context + (temp_1 << 2));
        void* data_1 = context + (temp_1 << 2);
        int temp_2 = *(data_1 + 0x10);

        if (temp_2)
        {
            *(data_1 + 0x10) = temp_2 - 1;
            entity_val += 1;
            *(data_1 + 8) -= 1;
        }

        if (arg4)
            *arg4 = source_1;

        if (arg5)
            *arg5 = dest_1;

        if (arg6)
            *arg6 = flagP1;

        result = 1;
    }

    *arg2 = entity_val;

    if (arg3)
        *arg3 = context_val;

    return result;
}
