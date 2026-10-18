// IGI native provenance: Render_ClipMesh_8 - 0x004D5090 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D5090-Render_ClipMesh_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_ClipMesh_8.h"
int Render_ClipMesh_8(void* context)

{
    int* entity_val = *(context + 0x30);

    if (entity_val)
    {
        int data_3 = *entity_val * 0x28;
        int source_1 = *(*(data_3 + &g_a5390c) + entity_val);
        *(data_3 + 0xa53900) -= 1;
        int data_7 = **(context + 0x30) * 0x28;
        int entity_1 = *(data_7 + 0xa53900);

        if (source_1 != entity_1)
        {
            int temp_2 = *(data_7 + &g_a538fc);
            int entity_4 = *(temp_2 + (entity_1 << 2)) * *(data_7 + 0xa53908) + *(data_7 + 0xa538f8);
            int context_2 = *(temp_2 + (source_1 << 2));
            int data_9 = *(*(data_7 + &g_a5390c) + entity_4);
            *(temp_2 + (source_1 << 2)) = *(temp_2 + (data_9 << 2));
            *(*(**(context + 0x30) * 0x28 + &g_a538fc) + (data_9 << 2)) = context_2;
            *(*(**(context + 0x30) * 0x28 + &g_a5390c) + entity_4) = source_1;
        }
    }

    int* temp_8 = g_a54634;
    int result = g_a54628 - 1;
    int dest_1 = *(temp_8 + context);
    g_a54628 = result;

    if (dest_1 != result)
    {
        int entity_5 = g_a54624;
        result = *(entity_5 + (result << 2)) * g_a54630 + g_a54620;
        int source_3 = *(entity_5 + (dest_1 << 2));
        int temp_9 = *(temp_8 + result);
        *(entity_5 + (dest_1 << 2)) = *(entity_5 + (temp_9 << 2));
        *(g_a54624 + (temp_9 << 2)) = source_3;
        *(g_a54634 + result) = dest_1;
    }

    return result;
}
