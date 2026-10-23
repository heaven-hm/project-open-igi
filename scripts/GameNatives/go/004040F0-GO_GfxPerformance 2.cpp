// IGI native provenance: GOGfxPerformance - 0x004040F0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004040F0-GO_GfxPerformance.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_GfxPerformance.h"
int GOGfxPerformance(int context, float arg2 )

// Sig: int GOGfxPerformance(int flags)
// Note: Sets graphics performance/detail flags for the current profile. Param:
// flags = bitmask from the detail-level mapping (see
// Config_GraphicOptionsGetPerfFlagsFromLevel). Use in config scripts. Returns
// 1.
{
    int data_1 = Script_GetArg(arg2, 0);
    int temp_val = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    *(temp_val * 0x564 + &g_bc1e3c) = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
