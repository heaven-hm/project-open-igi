// IGI native provenance: Sys_AllocateMemory_3 - 0x004B0EE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0EE0-Sys_AllocateMemory_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_AllocateMemory_3.h"
int Sys_AllocateMemory_3(int context, int arg2)

{
    int dest_val = 0;
    char result;

    for (int* i = &g_a950a0; i < &g_a960e0; )
    {
        result = i[0x81];

        if (result)
        {
            int data_val = *i;
            int item_8 = dest_val;
            int item_4_1 = data_val;
            result = context(&item_8, arg2);
        }

        i = &i[0x82];
        dest_val += 1;
    }

    return result;
}
