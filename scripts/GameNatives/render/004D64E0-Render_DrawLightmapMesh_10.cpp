// IGI native provenance: Render_DrawLightmapMesh_10 - 0x004D64E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D64E0-Render_DrawLightmapMesh_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D64E0-Render_DrawLightmapMesh_10.hpp"
long double Render_DrawLightmapMesh_10(void* context, int arg2)

{
    return **(Render_DrawBoneMesh_10(context, arg2) + 0xc);
}
