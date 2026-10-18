// IGI native provenance: MenuManager_PushScreen - 0x00417690 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00417690-MenuManager_PushScreen.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_PushScreen.hpp"
int MenuManager_PushScreen(int context, void* arg2, float arg3)

// Sig: void MenuManager_PushScreen(int screenId)
// Note: Pushes a screen onto the menu stack: allocates the screen object
// (objsize 0x2898) and stores it in the manager's screen stack at mgr+0x27EC
// (verified in igi.exe). Param: screenId = retail screen id. Remember to
// MenuManager_PopScreen when done.
{
    int item_24 = 0;
    *(arg2 + (*(arg2 + 0x27e8) << 2) + 0x27ec) = *(arg2 + 0x20);
    *(arg2 + 0x27e8) += 1;
    *(arg2 + 0x20) = Script_GetArg(arg3, item_24);
    *(arg2 + 0x2c) = 0;
    *(arg2 + 0x28) = 6;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
