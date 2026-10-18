// IGI native provenance: Config_GraphicOptionsSetDevice - 0x004047F0 - CFG score 79.2% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004047F0-Config_GraphicOptionsSetDevice.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004047F0-Config_GraphicOptionsSetDevice.hpp"
int Config_GraphicOptionsSetDevice(int context, float arg2 )

// Sig: void Config_GraphicOptionsSetDevice(int deviceIndex)
// Note: Applies a render device to the active profile. Param: deviceIndex =
// index from Config_FillRenderDeviceListBox. Use in video-options screen
// script.
{
    int context_3 = Script_GetArg(arg2, 0) * 0x404;
    int dest_val = context_3 + 0x567d18;
    int i = 0xffffffff;
    void* temp_1 = Config_GetActiveGraphicOptions() + 0x113;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_val = ~i;
    int source_1;
    int dest_3;
    dest_3 = memcpy(temp_1, dest_val - entity_val, entity_val & 0xfffffffc);
    memcpy(dest_3, source_1, entity_val & 3);
    void* dest_4 = context_3 + &g_567c98;
    int i_1 = 0xffffffff;
    void* temp_3 = Config_GetActiveGraphicOptions() + 0x13;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_4;
        dest_4 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int entity_4 = ~i_1;
    int item_18 = 0;
    int item_10 = 0;
    int source_3;
    int dest_7;
    dest_7 = memcpy(temp_3, dest_4 - entity_4, entity_4 & 0xfffffffc);
    int item_c = 0;
    memcpy(dest_7, source_3, entity_4 & 3);
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
