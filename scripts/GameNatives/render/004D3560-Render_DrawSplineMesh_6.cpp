// IGI native provenance: Render_DrawSplineMesh_6 - 0x004D3560 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D3560-Render_DrawSplineMesh_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D3560-Render_DrawSplineMesh_6.hpp"
int* Render_DrawSplineMesh_6(int context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)

{
    int* result = Render_DrawLightmapMesh_6();
    Render_SkinBones_6(result, 0, context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    return result;
}
