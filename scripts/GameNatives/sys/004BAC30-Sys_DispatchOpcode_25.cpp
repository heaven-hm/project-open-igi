// IGI native provenance: Sys_DispatchOpcode_25 - 0x004BAC30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BAC30-Sys_DispatchOpcode_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BAC30-Sys_DispatchOpcode_25.hpp"
void* Sys_DispatchOpcode_25(int context)

{
    int i_1 = context;
    int* data_2 = Mem_Alloc((i_1 * 3 + 3) << 2, 4);
    data_2[1] = 0;
    *data_2 = 0;
    void* result = HeapList_Insert(&g_a43ea0, data_2);
    int* source_val = &data_2[3];
    data_2[2] = source_val;

    if (i_1 > 0)
    {
        int i;

        do
        {
            source_val[1] = 0;
            *source_val = 0;
            result = HeapList_Insert(&g_a43eb0, source_val);
            source_val = &source_val[3];
            i = i_1;
            i_1 -= 1;
        } while (i != 1);
    }

    return result;
}
