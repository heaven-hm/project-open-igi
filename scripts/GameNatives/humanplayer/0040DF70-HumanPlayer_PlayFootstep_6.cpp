// IGI native provenance: HumanPlayer_PlayFootstep_6 - 0x0040DF70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040DF70-HumanPlayer_PlayFootstep_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_6.hpp"
void* HumanPlayer_PlayFootstep_6(void* context, void* arg2)

{
    if (!*(arg2 + 0x33d))
    {
        int data_1 = Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
        *(context + 0x14) = sub_40dfc0;
        return data_1;
    }

    Human_StateAnimation(arg2, 0x13, 0.200000003f);
    *(context + 0x14) = sub_40dfc0;
    return context;
}
