// IGI native provenance: HumanPlayer_PlayFootstep_4 - 0x0040BDB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040BDB0-HumanPlayer_PlayFootstep_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_4.hpp"
float HumanPlayer_PlayFootstep_4(void* context, void* arg2) {
    *(arg2 + 0x33d) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    int temp_1 = *(arg2 + 0x730);
    float result = *(arg2 + 0x520) + *(arg2 + 0x728);
    long double fpu_val_7_3 = *(arg2 + 0x524) + *(arg2 + 0x72c);
    int item_4 = *(arg2 + 0x528);
    *(arg2 + 0x664) = result;
    int item_4_1 = temp_1;
    *(arg2 + 0x668) = fpu_val_7_3;
    *(arg2 + 0x66c) = temp_1;
    *(context + 0x14) = HumanPlayer_PlaySound_4;
    return result;
}
