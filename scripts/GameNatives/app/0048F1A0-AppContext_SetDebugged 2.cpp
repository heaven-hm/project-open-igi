// IGI native provenance: AppContext_SetDebugged - 0x0048F1A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F1A0-AppContext_SetDebugged.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AppContext_SetDebugged.h"
int AppContext_SetDebugged(char context)

// Sig: void AppContext_SetDebugged(char state)
// Note: Sets the app-level DEBUGGED flag (global 0x5C8BF8). Param: state = 1
// mark debugger attached/debug mode, 0 clear. BlankName's IGIPatch calls this
// by name. Use to toggle engine debug paths.
{
    int result = context;
    g_5c8bf8 = result;
    return result;
}
