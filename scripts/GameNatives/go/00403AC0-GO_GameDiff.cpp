// IGI native provenance: GOGameDiff - 0x00403AC0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00403AC0-GO_GameDiff.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00403AC0-GO_GameDiff.hpp"
int GOGameDiff(int context, float arg2 )

// Sig: int GOGameDiff(int difficulty)
// Note: Sets game difficulty for the current profile. Param: difficulty = 0
// easy, 1 medium, 2 hard (retail order). Use in config scripts. Returns 1.
{
    int data_1 = Script_GetArg(arg2, 0);
    int temp_val = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    *(temp_val * 0x564 + &g_bc1e24) = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
