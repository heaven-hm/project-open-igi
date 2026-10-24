// IGI native provenance: AppContext_SetTerrainLightmapsUsed - 0x0048F260 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F260-AppContext_SetTerrainLightmapsUsed.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AppContext_SetTerrainLightmapsUsed.h"
int AppContext_SetTerrainLightmapsUsed(char context)

// Sig: void AppContext_SetTerrainLightmapsUsed(char used)
// Note: Enables/disables TERRAIN lightmaps globally (global 0x5C8BE8). Param:
// used = 1 lit terrain, 0 unlit. Pair with AppContext_SetLightmapsUsed for full
// lighting control.
{
    int result = context;
    g_5c8be8 = result;
    return result;
}
