// IGI native provenance: Config_FillRenderDeviceListBox - 0x004046A0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004046A0-Config_FillRenderDeviceListBox.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_FillRenderDeviceListBox.h"
int Config_FillRenderDeviceListBox(int context)

// Sig: void Config_FillRenderDeviceListBox()
// Note: Fills the currently-active menu list box with installed render devices
// (Direct3D device GUIDs/names). Use inside a list box collect-items script on
// the video-options screen.
{
    int i = 0;

    if (g_567c90 > 0)
    {
        int dest_1 = 0x567d18;

        do
        {
            ListBox_AddItem(dest_1, i);
            i += 1;
            dest_1 += 0x404;
        } while (i < g_567c90);
    }

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
