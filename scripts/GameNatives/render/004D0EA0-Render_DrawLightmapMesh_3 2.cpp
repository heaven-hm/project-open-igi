// IGI native provenance: Render_DrawLightmapMesh_3 - 0x004D0EA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0EA0-Render_DrawLightmapMesh_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawLightmapMesh_3.h"
void Render_DrawLightmapMesh_3(void* context, int* arg2)

{
    int* entity_val = arg2;

    if (!entity_val)
        return;

    int i = 0;
    void* data_val = *(context + 0xc);

    if (*(data_val + 0x10) <= 0)
        return;

    int temp_2 = 0;

    do
    {
        int context_1 = *entity_val;
        i += 1;
        entity_val = &entity_val[1];
        *(*(data_val + 0x14) + temp_2 + 4) = context_1;
        data_val = *(context + 0xc);
        temp_2 += 0x14;
    } while (i < *(data_val + 0x10));
}
