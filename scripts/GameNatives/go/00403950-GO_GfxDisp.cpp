// IGI native provenance: GOGfxDisp - 0x00403950 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00403950-GO_GfxDisp.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00403950-GO_GfxDisp.hpp"
int GOGfxDisp(int context, float arg2 )

// Sig: int GOGfxDisp(int displayMode)
// Note: Sets display-mode selection for the current profile. Param: displayMode
// = mode index from the retail display list (see Display_GetActiveMode/display
// table). Use in config scripts. igi.exe registers it with argc=3 at its site;
// the extra args go unused by the setter itself.
{
    int data_val = Script_GetArg(arg2, 0);
    int temp_val = g_bc2380;
    int item_2c = 1;
    float item_30 = arg2;
    *(temp_val * 0x564 + &g_bc1e30) = data_val;
    *(g_bc2380 * 0x564 + &g_bc1e34) = Script_GetArg(item_30, item_2c);
    int data_2 = Script_GetArg(arg2, 2);
    int temp_3 = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    *(temp_3 * 0x564 + &g_bc1e38) = data_2;
    memcpy(context, &item_18, 0x18);
    return context;
}
