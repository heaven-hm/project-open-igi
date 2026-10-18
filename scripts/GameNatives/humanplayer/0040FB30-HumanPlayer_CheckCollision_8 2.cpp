// IGI native provenance: HumanPlayer_CheckCollision_8 - 0x0040FB30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040FB30-HumanPlayer_CheckCollision_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckCollision_8.h"
int HumanPlayer_CheckCollision_8(void* context, void* arg2)

{
    int dest_val = *(arg2 + 0x4ec);
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    int result = Bones_PlayAnimation(dest_val + 0x68, 0,
        *(HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1) + 0x684), 0, 0.0f);
    *(arg2 + 0xd6c) = 0;
    *(context + 0x14) = sub_40fb90;
    return result;
}
