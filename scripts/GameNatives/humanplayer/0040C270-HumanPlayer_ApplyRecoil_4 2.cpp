// IGI native provenance: HumanPlayer_ApplyRecoil_4 - 0x0040C270 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040C270-HumanPlayer_ApplyRecoil_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_ApplyRecoil_4.h"
int* HumanPlayer_ApplyRecoil_4(int context, void* arg2)

{
    int item_1c = 0xc;
    int item_14 = 0x46;
    int item_18 = *(arg2 + 0xfc);
    int item_10 = 0x46a00000;
    int item_c = DynObj_UpdateDynTree_4(arg2);
    int item_8 = context;
    void* item_4 = arg2;
    return AI_EventBroadcast(&item_1c);
}
