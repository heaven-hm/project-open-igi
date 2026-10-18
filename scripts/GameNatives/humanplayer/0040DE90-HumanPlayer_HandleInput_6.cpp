// IGI native provenance: HumanPlayer_HandleInput_6 - 0x0040DE90 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040DE90-HumanPlayer_HandleInput_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_HandleInput_6.hpp"
void** HumanPlayer_HandleInput_6(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    void** result = HumanPlayer_CheckCollision_9(arg2, 0.0f);
    memset(arg2 + 0x664, 0, 0x18);
    *(context + 0x14) = HumanPlayer_ApplyPhysics_6;
    return result;
}
