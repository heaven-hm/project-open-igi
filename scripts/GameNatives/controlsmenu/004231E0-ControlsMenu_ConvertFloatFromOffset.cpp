// IGI native provenance: ControlsMenu_ConvertFloatFromOffset - 0x004231E0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004231E0-ControlsMenu_ConvertFloatFromOffset.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004231E0-ControlsMenu_ConvertFloatFromOffset.hpp"
int ControlsMenu_ConvertFloatFromOffset(int context, int arg2 )

// Sig: float ControlsMenu_ConvertFloatFromOffset(int offset)
// Note: Converts a controls-menu slider/storage OFFSET into its float value
// (0.0-1.0 range used by the control configuration bars). Param: offset = raw
// integer offset stored for that control binding. Use when reading/writing
// control bindings from script.
{
    int item_2c = 0;
    float item_30 = arg2;
    long double fpu_val_7_1 = Script_GetArg(item_30, item_2c) / 0x11;
    char* item_8 = &g_567c74;
    int item_18 = fpu_val_7_1;
    double item_10 = fpu_val_7_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
