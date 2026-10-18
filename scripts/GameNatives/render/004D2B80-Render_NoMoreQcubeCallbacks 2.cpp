// IGI native provenance: Render_NoMoreQcubeCallbacks - 0x004D2B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D2B80-Render_NoMoreQcubeCallbacks.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_NoMoreQcubeCallbacks.h"
int* Render_NoMoreQcubeCallbacks(int context)

{
    int entity_val = 0;

    for (int* i = &g_a4eeac; i < 0xa4eefc; )
    {
        if (!*i)
        {
            int temp_val = g_a538e4;
            g_a538e4 = (entity_val << 3) + &g_a4eea8;
            *((entity_val << 3) + &g_a4eea8) = temp_val;
            *((entity_val << 3) + &g_a4eeac) = context;
            return (entity_val << 3) + &g_a4eea8;
        }

        i = &i[2];
        entity_val += 1;
    }

    Error_Show("No more QCube callbacks available.");

    while (true)
        /* nop */
}
