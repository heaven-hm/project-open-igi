// IGI native provenance: DynObj_InvalidCategory - 0x004C7290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C7290-DynObj_InvalidCategory.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_InvalidCategory.h"
int DynObj_InvalidCategory(int context, short arg2)

{
    unsigned int data_1 = arg2 << 2;
    int temp_val;

    if (*(data_1 + &g_a982e0) || *(data_1 + &g_a988e0) || *(data_1 + &g_a9c4e0)
            || *(data_1 + &g_a9cae0))
        temp_val = 1;
    else
        temp_val = 0;

    int data_2;

    if (*(data_1 + &g_a9ace0) || *(data_1 + &g_a9a0e0) || *(data_1 + &g_a9a6e0)
            || *(data_1 + &g_a99ae0))
        data_2 = 1;
    else
        data_2 = 0;

    if (context == 4)
        return context;

    if (context == 8)
        return 5;

    if (context == 2)
    {
        if (temp_val)
        {
            int data_5 = -(data_2);
            return data_5 - data_5 + context;
        }

        int data_8 = -(data_2);
        int data_9 = data_8 - data_8;
        data_9 &= 0xfe;
        return data_9 + 3;
    }

    if (context == 1)
        return 0;

    Error_Show("Invalid category");

    while (true)
        /* nop */
}
