// IGI native provenance: AppContext_SetLightmapsUsed - 0x0048F240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F240-AppContext_SetLightmapsUsed.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048F240-AppContext_SetLightmapsUsed.hpp"
int AppContext_SetLightmapsUsed(char context)

// Sig: void AppContext_SetLightmapsUsed(char used)
// Note: Enables/disables OBJECT lightmaps globally (global 0x5C8BE4) - the
// verified retail lightmap setter. Param: used = 1 use object lightmaps, 0
// flat-lit. Use for graphics enhancers (this is what IGIPatch's lightmap
// toggles call).
{
    int result = context;
    g_5c8be4 = result;
    return result;
}
