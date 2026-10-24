// IGI native provenance: Render_DrawLightmapMesh_5 - 0x004D2EC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D2EC0-Render_DrawLightmapMesh_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D2EC0-Render_DrawLightmapMesh_5.hpp"
int* Render_DrawLightmapMesh_5(void* context, int arg2)

{
    int entity_val;
    int item_4 = entity_val;
    int* result = g_a54638;
    void* context_val = nullptr;
    int i = 0;

    if (*result > 0)
    {
        item_4 = 0;

        do
        {
            if (*(result[1] + (i << 2)) == arg2)
            {
                int* source_1 = context_val + g_a5467c;
                Render_SkinBones_5(source_1, context);
                *source_1 = i;
                int* entity_6 = *(g_a54638 + 8) + item_4;
                source_1[2] = *entity_6;
                source_1[3] = entity_6[1];
                source_1[4] = entity_6[2];
                long double fpu_val_7_1 = source_1[4];
                long double fpu_val_6_1 = source_1[3];
                long double fpu_val_5_1 = source_1[2];
                source_1[5] = sqrtl(fpu_val_5_1 * fpu_val_5_1 + fpu_val_6_1 * fpu_val_6_1 + fpu_val_7_1 * fpu_val_7_1);
                Render_DrawLightmapMesh_5(source_1, i);
                result = g_a54638;
            }

            i += 1;
            context_val += 0x84;
            item_4 += 0xc;
        } while (i < *result);
    }

    return result;
}
