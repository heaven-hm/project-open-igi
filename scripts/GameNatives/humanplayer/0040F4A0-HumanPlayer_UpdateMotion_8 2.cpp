// IGI native provenance: HumanPlayer_UpdateMotion_8 - 0x0040F4A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040F4A0-HumanPlayer_UpdateMotion_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateMotion_8.h"
int HumanPlayer_UpdateMotion_8(void* context, void* arg2)

{
    int dest_val = *(arg2 + 0x4ec);
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    void* data_2 = HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    *(arg2 + 0x4d2) = 1;
    int result = Bones_PlayAnimation(dest_val + 0x68, 0, *(data_2 + 0x688), 0, 0.0f);
    *(context + 0x14) = sub_40fd70;
    return result;
}
