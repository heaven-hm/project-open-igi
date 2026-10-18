// IGI native provenance: GO_ActiveMission - 0x00403750 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00403750-GO_ActiveMission.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_ActiveMission.h"
int GO_ActiveMission(int context, float arg2 )

// Sig: int GO_ActiveMission(int mission)
// Note: Sets the active (last-played) mission number for the current player
// profile. Param: mission = mission index 1..14. Use in profile/config scripts
// after GO_Player. Returns 1.
{
    int data_1 = Script_GetArg(arg2, 0);
    int temp_val = g_bc2380;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    (&g_bc2384)[temp_val * 0x159] = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
