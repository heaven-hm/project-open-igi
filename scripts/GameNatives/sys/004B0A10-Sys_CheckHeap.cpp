// IGI native provenance: Sys_CheckHeap - 0x004B0A10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0A10-Sys_CheckHeap.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0A10-Sys_CheckHeap.hpp"
int Sys_CheckHeap(char* context, int arg2, int arg3, int arg4)

{
    if (g_9422e4 <= arg4)
    {
        if (arg2)
            *context = 0;
    }
    else if (arg2)
    {
        char* i = nullptr;

        if (arg2 - 1 > 0)
        {
            do
            {
                int entity_1 = g_9422e0;

                if (i >= *(entity_1 + (arg4 << 3) + 4) - 1)
                    break;

                char* entity_3 = *(entity_1 + (arg4 << 3)) + i;
                i = &i[1];
                *(i + context - 1) = entity_3[arg3];
            } while (i < arg2 - 1);
        }

        *(i + context) = 0;
        return 1;
    }

    return 0;
}
