// IGI native provenance: Sys_PopStack_17 - 0x004B6D60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6D60-Sys_PopStack_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B6D60-Sys_PopStack_17.hpp"
int* Sys_PopStack_17(void* context, int arg2)

{
    if (arg2)
    {
        int data_val = 0;
        data_val = *(context + 0x14);
        int* result = *(context + 8) + data_val * 0x24 + 4;
        *result |= 0x10000;
        return result;
    }

    int data_2 = 0;
    data_2 = *(context + 0x14);
    int temp_val = data_2 * 9;
    int data_3 = *(context + 8);
    *(data_3 + (temp_val << 2) + 4) &= 0xfffeffff;
    return data_3 + (temp_val << 2) + 4;
}
