// IGI native provenance: Sys_CheckHeap_19 - 0x004B74D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B74D0-Sys_CheckHeap_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_CheckHeap_19.h"
void* Sys_CheckHeap_19(void* context, void* arg2)

{
    if (arg2 && context)
    {
        int source_1 = *(arg2 + 8);

        if ((~(source_1 - 1) & source_1) != source_1)
            g_a43e5c = 1;
        else
        {
            void* data_5 = context - arg2;

            if (data_5 >= *(arg2 + 4))
                g_a43e5c = 1;
            else
            {
                int entity_2 = *(context + 8);

                if ((~(entity_2 - 1) & entity_2) != entity_2 || entity_2 > source_1)
                    g_a43e5c = 1;
                else if (*(context + 4))
                    return ((entity_2 + data_5 + 0xf) & ~(entity_2 - 1)) + arg2;
            }
        }
    }

    return 0;
}
