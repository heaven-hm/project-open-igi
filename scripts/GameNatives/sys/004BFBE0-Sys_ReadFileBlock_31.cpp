// IGI native provenance: Sys_ReadFileBlock_31 - 0x004BFBE0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004BFBE0-Sys_ReadFileBlock_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BFBE0-Sys_ReadFileBlock_31.hpp"
void* Sys_ReadFileBlock_31(void* context, void* arg2)

{
    int i = 0xffffffff;
    int dest_val = *(arg2 + 0x14);

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int* data_val = Mem_Alloc(~i + 0x28, 4);
    data_val[1] = 0;
    *data_val = 0;
    data_val[4] = &data_val[0xa];
    data_val[2] = *(arg2 + 0xc);
    data_val[3] = *(arg2 + 0x10);
    data_val[8] = *(arg2 + 0x20);
    data_val[6] = *(arg2 + 0x18);
    data_val[7] = *(arg2 + 0x1c);
    int dest_1 = *(arg2 + 0x14);
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_1;
        dest_1 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int entity_4 = ~i_1;
    int source_2;
    int dest_4;
    dest_4 = memcpy(&data_val[0xa], dest_1 - entity_4, entity_4 & 0xfffffffc);
    memcpy(dest_4, source_2, entity_4 & 3);
    return HeapList_Insert(context, data_val);
}
