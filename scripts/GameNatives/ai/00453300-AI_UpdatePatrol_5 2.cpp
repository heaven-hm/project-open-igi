// IGI native provenance: AI_UpdatePatrol_5 - 0x00453300 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453300-AI_UpdatePatrol_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_5.h"
int* AI_UpdatePatrol_5(int context, char arg2)

{
    if (!arg2)
    {
        int* source_1 = g_5bda28;
        int* data_2 = *source_1;

        if (data_2)
        {
            while (source_1[2] != context)
            {
                source_1 = data_2;
                data_2 = *data_2;

                if (!data_2)
                    return data_2;
            }

            HeapList_Remove(source_1);
            source_1[1] = 0;
            *source_1 = 0;
            HeapList_Insert(&g_57d758, source_1);
        }
    }
    else
    {
        int* source_val = g_57d758;

        if (*source_val)
        {
            HeapList_Remove(source_val);
            source_val[1] = 0;
            *source_val = 0;
            HeapList_Insert(&g_5bda28, source_val);
            source_val[2] = context;
            return source_val;
        }
    }

    return 0;
}
