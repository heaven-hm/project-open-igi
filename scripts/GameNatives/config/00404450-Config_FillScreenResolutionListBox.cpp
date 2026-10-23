// IGI native provenance: Config_FillScreenResolutionListBox - 0x00404450 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00404450-Config_FillScreenResolutionListBox.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_FillScreenResolutionListBox.h"
int Config_FillScreenResolutionListBox(int context, float arg2 )

// Sig: void Config_FillScreenResolutionListBox(int deviceIndex)
// Note: Fills the currently-active menu list box with the screen resolutions
// available for a device. Param: deviceIndex = render-device index. Use only
// inside a menu list box 'collect items' script so the items land in that
// control.
{
    int i = 0;
    int dest_3 = Script_GetArg(arg2, 0) * 0x404;
    int* source_val = dest_3 + 0x567d98;

    if (*(dest_3 + &g_568098) > 0)
    {
        do
        {
            int item_128_1 = source_val[2];
            int item_12c_1 = source_val[1];
            int item_130 = *source_val;
            void item_100;
            sprintf(&item_100, "%word_data%word_data%d");
            ListBox_AddItem(&item_100, (source_val[2] << 0xd | *source_val) << 0xd | source_val[1]);
            source_val = &source_val[3];
            i += 1;
        } while (i < *(dest_3 + &g_568098));
    }

    int item_118 = 0;
    int item_110 = 0;
    int item_10c = 0;
    char* item_108 = &g_567c74;
    memcpy(context, &item_118, 0x18);
    return context;
}
