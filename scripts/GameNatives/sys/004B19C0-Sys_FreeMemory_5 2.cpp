// IGI native provenance: Sys_FreeMemory_5 - 0x004B19C0 - CFG score 80.0% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B19C0-Sys_FreeMemory_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_FreeMemory_5.h"
void** Sys_FreeMemory_5(int context)

{
    int dest_val = context;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    void** result = Mem_Alloc(~i + 0x1c, 4);
    memset(result, 0, 0x1c);
    result[1] = 0;
    *result = nullptr;
    HeapList_Insert(&g_943970, result);
    int dest_2 = context;
    int i_1 = 0xffffffff;
    result[2] = &result[7];

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_2;
        dest_2 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int entity_2 = ~i_1;
    int source_2;
    int dest_5;
    dest_5 = memcpy(&result[7], dest_2 - entity_2, entity_2 & 0xfffffffc);
    memcpy(dest_5, source_2, entity_2 & 3);
    return result;
}
