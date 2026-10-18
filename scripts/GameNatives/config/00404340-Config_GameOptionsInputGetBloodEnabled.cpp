// IGI native provenance: Config_GameOptionsInputGetBloodEnabled - 0x00404340 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404340-Config_GameOptionsInputGetBloodEnabled.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00404340-Config_GameOptionsInputGetBloodEnabled.hpp"
int Config_GameOptionsInputGetBloodEnabled(int context)

// Sig: int Config_GameOptionsInputGetBloodEnabled()
// Note: RETURNS the blood-enabled flag from the game-options config context
// (1/0). Takes no args.
{
    int item_18 = *(Config_GetActivePlayerProfileRecord() + 8);
    char* item_8 = &g_567c74;
    double item_10 = *(Config_GetActivePlayerProfileRecord() + 8);
    memcpy(context, &item_18, 0x18);
    return context;
}
