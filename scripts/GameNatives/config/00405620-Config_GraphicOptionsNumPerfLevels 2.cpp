// IGI native provenance: Config_GraphicOptionsNumPerfLevels - 0x00405620 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00405620-Config_GraphicOptionsNumPerfLevels.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_GraphicOptionsNumPerfLevels.h"
int Config_GraphicOptionsNumPerfLevels(int context)

// Sig: int Config_GraphicOptionsNumPerfLevels()
// Note: RETURNS the number of graphics detail levels the engine supports
// (constant). Use to build the detail-level slider/list.
{
    int item_18 = 7;
    int item_10 = 0;
    int item_c = 0x401c0000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
