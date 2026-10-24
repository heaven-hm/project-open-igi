// IGI native provenance: AppContext_SetDebugtextState - 0x0048F1E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F1E0-AppContext_SetDebugtextState.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AppContext_SetDebugtextState.h"
int AppContext_SetDebugtextState(char context)

// Sig: void AppContext_SetDebugtextState(char state)
// Note: Toggles on-screen DEBUG TEXT output (global 0x5C8BF4). Param: state = 1
// show engine debug text, 0 hide. Handy while reverse-engineering other
// systems.
{
    int result = context;
    g_5c8bf4 = result;
    return result;
}
