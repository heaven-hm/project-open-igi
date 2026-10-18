// IGI native provenance: HumanPlayer_WireSlide1 - 0x0040DBA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040DBA0-HumanPlayer_WireSlide1.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_WireSlide1.hpp"
struct status_msg* HumanPlayer_WireSlide1(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    HumanPlayer_CheckCollision_9(arg2, 0.0f);
    AI_CombatTarget_20(arg2);
    struct status_msg* result = SoundSys_SetMasterVolume_7(arg2, "wire_slide_1");
    *(arg2 + 0xdb4) = result;
    *(context + 0x14) = HumanPlayer_CheckGround_6;
    return result;
}
