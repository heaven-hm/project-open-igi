// IGI native provenance: MenuManager_DeactivatePopuScreen - 0x00417960 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00417960-MenuManager_DeactivatePopuScreen.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_DeactivatePopuScreen.hpp"
int MenuManager_DeactivatePopuScreen(int context, void* arg2)

// Sig: void MenuManager_DeactivatePopuScreen()
// Note: Deactivates the currently-active popup screen and returns to the screen
// beneath. Takes no args. Pair with MenuManager_ActivatePopupScreen. NOTE: name
// mirrors the retail registration string exactly, including its "Popu" typo -
// do not "fix" it against the binary.
{
    *(arg2 + 0x28) = 8;
    *(arg2 + 0x2c) = 0xf;
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
