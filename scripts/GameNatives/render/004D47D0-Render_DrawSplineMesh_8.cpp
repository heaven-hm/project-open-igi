// IGI native provenance: Render_DrawSplineMesh_8 - 0x004D47D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D47D0-Render_DrawSplineMesh_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D47D0-Render_DrawSplineMesh_8.hpp"
int* Render_DrawSplineMesh_8(int* context, int arg2, int arg3, int arg4)

{
    int* result = Render_ClipMesh_7();
    int* temp_val = *(*(context * 0x28 + &g_a538f4) + (arg3 << 2));
    int* data_1 = Render_SetupLighting_8();
    int entity_1 = arg4;
    *(&entity_1 + 1) |= 0x80;
    Render_BindTexture_8(data_1, temp_val, entity_1, context, arg2, arg3);
    Render_DrawRigidMesh_8(result, Render_DrawLightmapMesh_8, sub_4d5180, Render_SkinBones_8, sub_4d5080, data_1);
    return result;
}
