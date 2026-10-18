// IGI native provenance: Game_SetMission - 0x00415890 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00415890-Game_SetMission.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_SetMission.h"
int Game_SetMission(int context, float arg2 )

// Sig: void Game_SetMission(int mission)
// Note: Selects the mission the game will run/load next. Param: mission =
// mission index 1..14. Use before Level_Start/level load flow when jumping
// straight into a mission.
{
    g_539560 = Script_GetArg(arg2, 0);
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
