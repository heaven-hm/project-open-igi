// IGI native provenance: MenuManager_LeaveMenus - 0x00417790 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00417790-MenuManager_LeaveMenus.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuManager_LeaveMenus.hpp"
int MenuManager_LeaveMenus(int context, void* arg2, float arg3)

// Sig: void MenuManager_LeaveMenus(int param1, int param2)
// Note: Leaves/closes the whole menu system returning to gameplay. Params: two
// ints observed in retail calls (pass 0,0). Use to exit from custom menus.
{
    int data_val = Script_GetArg(arg3, 0);
    int item_2c = 1;
    float item_30 = arg3;
    int data_1 = Script_GetArg(item_30, item_2c);

    if (!FX_DebrisSpawn_11() || data_1 != 9)
    {
        *(arg2 + 0x20) = data_val;
        *(arg2 + 0x28) = 6;
        *(arg2 + 0x2c) = data_1;
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
