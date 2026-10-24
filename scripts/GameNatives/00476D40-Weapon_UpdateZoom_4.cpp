// IGI native provenance: Weapon_UpdateZoom_4 - 0x00476D40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00476D40-Weapon_UpdateZoom_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00476D40-Weapon_UpdateZoom_4.hpp"
int* Weapon_UpdateZoom_4(int context, int arg2, int arg3, int arg4)

{
    int item_1c = arg2;
    int item_10 = arg4;
    int item_14 = arg3;
    int item_18 = 0xffffffff;
    int item_c = context + 0x20;
    int item_8 = context;
    int item_4 = 0;
    return AI_EventBroadcast(&item_1c);
}
