// IGI native provenance: SmoothQTask_OnEvent - 0x0051F270 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051F270-SmoothQTask_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SmoothQTask_OnEvent.hpp"
int SmoothQTask_OnEvent(void* context)

{
    void item_c0;
    Script_ParseEnumString_6(&item_c0, context);
    void* item_d0 = &item_c0;
    *(context + 0xf8) = 0;
    void item_80;
    sprintf(&item_80, "%s.vRealValue");
    Game_DataSymbolRemoveThunk(&item_80);
    void* item_e0 = &item_c0;
    sprintf(&item_80, "%s.vIdealValue");
    int result = Game_DataSymbolRemoveThunk(&item_80);
    *(context + 0xf8) = 0;
    return result;
}
