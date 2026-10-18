// IGI native provenance: HumanPlayer_PlayFootstep_3 - 0x0040AA00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040AA00-HumanPlayer_PlayFootstep_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_3.hpp"
int HumanPlayer_PlayFootstep_3(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    *(context + 0x14) = sub_40aa30;
    return result;
}
