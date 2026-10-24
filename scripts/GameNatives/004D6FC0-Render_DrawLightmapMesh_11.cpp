// IGI native provenance: Render_DrawLightmapMesh_11 - 0x004D6FC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D6FC0-Render_DrawLightmapMesh_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D6FC0-Render_DrawLightmapMesh_11.hpp"
int* Render_DrawLightmapMesh_11(int* context, int* arg2, int* arg3, int arg4, int arg5)

{
    void* temp_1 = *(*(*arg3 * 0x28 + &g_a538f4) + (arg4 << 2));

    if (context)
    {
        int* source_3 = *(*(temp_1 + 8) + 4) + arg5 * 0x28;
        *context = *source_3;
        context[1] = source_3[1];
        context[2] = source_3[2];
    }

    if (arg2)
        *arg2 = *(*(*(temp_1 + 8) + 4) + arg5 * 0x28 + 0xc);

    return arg2;
}
