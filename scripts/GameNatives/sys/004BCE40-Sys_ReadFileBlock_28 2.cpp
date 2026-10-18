// IGI native provenance: Sys_ReadFileBlock_28 - 0x004BCE40 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004BCE40-Sys_ReadFileBlock_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_28.h"
int Sys_ReadFileBlock_28(void* context, int arg2, int arg3)

{
    int dest_val = arg2;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int dest_1 = *(context + 0x28);
    int data_1 = *(context + 0x1c) - 1;
    *(context + 0x1c) = data_1;
    *(dest_1 + (data_1 << 2)) = arg3;
    int entity_3 = *(context + 0x24);
    unsigned int context_2 = ((~i + 3) & 0xfffffffc) >> 2;
    int dest_3 = *(context + 0x18) - context_2;
    *(context + 0x18) = dest_3;
    int dest_4 = arg2;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_4;
        dest_4 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int entity_4 = ~i_1;
    int source_2;
    int dest_7;
    dest_7 = memcpy(entity_3 + (dest_3 << 2), dest_4 - entity_4, entity_4 & 0xfffffffc);
    memcpy(dest_7, source_2, entity_4 & 3);
    int entity_8 = *(context + 0x24);
    int result = *(context + 0x18) - 1;
    *(context + 0x18) = result;
    *(entity_8 + (result << 2)) = context_2;
    return result;
}
