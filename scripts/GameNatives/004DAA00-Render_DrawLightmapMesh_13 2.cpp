// IGI native provenance: Render_DrawLightmapMesh_13 - 0x004DAA00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DAA00-Render_DrawLightmapMesh_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawLightmapMesh_13.h"
int Render_DrawLightmapMesh_13()

{
    int result = g_b03f90;
    int entity_val = g_b03df0;

    if (result == entity_val)
        g_b03df0 = entity_val - 1;

    g_b03f90 -= 1;
    return result;
}
