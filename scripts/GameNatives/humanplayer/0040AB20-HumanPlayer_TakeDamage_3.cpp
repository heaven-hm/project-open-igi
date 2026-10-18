// IGI native provenance: HumanPlayer_TakeDamage_3 - 0x0040AB20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040AB20-HumanPlayer_TakeDamage_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_TakeDamage_3.hpp"
int HumanPlayer_TakeDamage_3(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    int result = HumanPlayer_ApplyRecoil_9(arg2, 4);
    *(context + 0x14) = sub_40ab60;
    return result;
}
