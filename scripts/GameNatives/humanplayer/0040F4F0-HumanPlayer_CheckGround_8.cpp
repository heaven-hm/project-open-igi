// IGI native provenance: HumanPlayer_CheckGround_8 - 0x0040F4F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040F4F0-HumanPlayer_CheckGround_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckGround_8.hpp"
int HumanPlayer_CheckGround_8(void* context, void* arg2)

{
    int flagP = *(arg2 + 0x4ec);
    void* data_1;
    int entity_val;
    data_1 = Table_GetAt(arg2 + 0x4c4, 0);
    int temp_val;
    void* data_2 = HumanPlayer_LoadParameters_13(data_1, temp_val, entity_val, data_1);
    Bones_PlayAnimation(flagP + 0x68, 0, *(data_2 + 0x684), 0, 0.0f);
    long double fpu_val_7_1 = **(SoundSys_EvaluateTrigger_18(data_2 + 0x61c) + 0x2d4) * 30.0f;
    *(arg2 + 0x4d8) = 0;
    *(arg2 + 0x4e0) = 0;
    *(arg2 + 0x4e4) = 0;
    int result = fpu_val_7_1;
    *(arg2 + 0x4dc) = result;
    *(context + 0x14) = HumanPlayer_GrenadeTick1;
    return result;
}
