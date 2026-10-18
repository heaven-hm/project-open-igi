// IGI native provenance: GO_GfxDevice - 0x00404010 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00404010-GO_GfxDevice.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00404010-GO_GfxDevice.hpp"
int GO_GfxDevice(int context, float arg2 )

// Sig: int GO_GfxDevice(int deviceIndex)
// Note: Sets render-device (D3D device/GUID) selection for the current profile.
// Param: deviceIndex = index into the render-device list filled by
// Config_FillRenderDeviceListBox. Use in config scripts. Returns 1.
{
    char* item_24 = 0x100;
    int data_3 = g_bc2380 * 0x45;
    int item_2c = 0;
    float item_30 = arg2;
    Sys_ResolveSymbol_23(item_30, item_2c, data_3 * 0x14 + &g_bc1e43, item_24);
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
