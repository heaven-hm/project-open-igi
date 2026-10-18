// IGI native provenance: HumanPlayer_PlaySound - 0x00408770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00408770-HumanPlayer_PlaySound.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlaySound.hpp"
int HumanPlayer_PlaySound(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    int result = HumanPlayer_ApplyRecoil_9(arg2, 1);
    *(context + 0x14) = sub_4087b0;
    return result;
}
