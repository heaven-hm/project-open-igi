// IGI native provenance: GO_InMouSens - 0x004038E0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004038E0-GO_InMouSens.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_InMouSens.h"
int GO_InMouSens(long double context , int arg2, float arg3 )

// Sig: float GO_InMouSens(float sensitivity)
// Note: Sets mouse sensitivity for the current profile. Param: sensitivity =
// float multiplier as saved by the options screen. Use in config scripts.
// Returns 1.
{
    Profile_ValueSet(arg3, 0.0f);
    int entity_val = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    (&g_bc2360)[entity_val * 0x159] = context;
    memcpy(arg2, &item_18, 0x18);
    return arg2;
}
