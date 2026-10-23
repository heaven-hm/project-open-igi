// IGI native provenance: GOInMouInv - 0x00403870 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00403870-GO_InMouInv.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_InMouInv.h"
int GOInMouInv(int context, float arg2 )

// Sig: int GOInMouInv(int invert)
// Note: Toggles inverted mouse-look for the current profile. Param: invert = 1
// invert Y axis, 0 normal. Use in config scripts. Returns 1.
{
    int data_1 = Script_GetArg(arg2, 0);
    int temp_val = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    *(temp_val * 0x564 + &g_bc235c) = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
