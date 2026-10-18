// IGI native provenance: HumanPlayer_UpdateMotion_7 - 0x0040E6E0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040E6E0-HumanPlayer_UpdateMotion_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateMotion_7.hpp"
int* HumanPlayer_UpdateMotion_7(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    memset(arg2 + 0x664, 0, 0x18);
    int* result = AI_UpdateSensors_19(arg2);
    *(context + 0x14) = HumanPlayer_CheckGround_7;
    return result;
}
