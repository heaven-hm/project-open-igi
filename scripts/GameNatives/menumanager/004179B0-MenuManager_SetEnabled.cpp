// IGI native provenance: MenuManager_SetEnabled - 0x004179B0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004179B0-MenuManager_SetEnabled.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_SetEnabled.hpp"
int MenuManager_SetEnabled(int context, void* arg2, float arg3)

// Sig: void MenuManager_SetEnabled(int enabled)
// Note: Enables/disables menu-manager input processing. Param: enabled = 1
// menus react to input, 0 frozen. Use to freeze menus during scripted
// sequences.
{
    if (Script_GetArg(arg3, 0) != 1)
        *(arg2 + 0x28) = 0x10;
    else
        *(arg2 + 0x28) = 0x11;

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
