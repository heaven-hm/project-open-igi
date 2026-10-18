// IGI native provenance: Render_DrawBoneMesh_6 - 0x004D34A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D34A0-Render_DrawBoneMesh_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawBoneMesh_6.h"
int Render_DrawBoneMesh_6(void* context)

{
    if (!*(context + 0x10))
    {
        if (*Render_SetupLighting_6(context, 0) != 0xffffffff)
            Render_BoneobjDeleteblendno(context, 0);

        if (*Render_SetupLighting_6(context, 1) != 0xffffffff)
            Render_BoneobjDeleteblendno(context, 1);
    }
    else
    {
        Render_ComputeNormals_6(context);
        Render_BoneobjDeleteblendno(context, 0);
        Render_BoneobjDeleteblendno(context, 1);
    }

    int* dest_val = g_a54674;
    int result = g_a54668 - 1;
    int entity_3 = *(dest_val + context);
    g_a54668 = result;

    if (entity_3 != result)
    {
        int temp_1 = g_a54664;
        result = *(temp_1 + (result << 2)) * g_a54670 + g_a54660;
        int source_2 = *(dest_val + result);
        int dest_1 = *(temp_1 + (entity_3 << 2));
        *(temp_1 + (entity_3 << 2)) = *(temp_1 + (source_2 << 2));
        *(g_a54664 + (source_2 << 2)) = dest_1;
        *(g_a54674 + result) = entity_3;
    }

    return result;
}
