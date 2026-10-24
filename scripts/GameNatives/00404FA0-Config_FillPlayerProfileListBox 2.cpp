// IGI native provenance: Config_FillPlayerProfileListBox - 0x00404FA0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00404FA0-Config_FillPlayerProfileListBox.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_FillPlayerProfileListBox.h"
int Config_FillPlayerProfileListBox(int context)

// Sig: void Config_FillPlayerProfileListBox()
// Note: Fills the current menu list box with all player profiles (name per row,
// row index = profile index). Use inside the profile-selection list box
// collect-items script.
{
    int i = 0;

    if (g_bc2380 > 0)
    {
        int* dest_1 = &g_bc28c8;

        do
        {
            Menu_DrawWidget_9(dest_1, i);
            i += 1;
            dest_1 = &dest_1[0x159];
        } while (i < g_bc2380);
    }

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
