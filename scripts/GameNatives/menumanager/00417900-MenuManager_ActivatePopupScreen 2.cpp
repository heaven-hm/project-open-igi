// IGI native provenance: MenuManager_ActivatePopupScreen - 0x00417900 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00417900-MenuManager_ActivatePopupScreen.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_ActivatePopupScreen.h"
int MenuManager_ActivatePopupScreen(int context, void* arg2, float arg3)

// Sig: void MenuManager_ActivatePopupScreen(int popupId)
// Note: Activates a popup screen on idx of the current menus (e.g. confirmation
// dialogs; retail scripts call MenuManager_ActivatePopupScreen(930)). Param:
// popupId = retail popup screen id. Use for custom confirmations.
{
    int data_1 = Script_GetArg(arg3, 0);
    int item_18 = 1;
    *(arg2 + 0x24) = data_1;
    *(arg2 + 0x28) = 0xe;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
