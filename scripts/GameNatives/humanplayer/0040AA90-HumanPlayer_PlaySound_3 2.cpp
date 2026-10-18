// IGI native provenance: HumanPlayer_PlaySound_3 - 0x0040AA90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040AA90-HumanPlayer_PlaySound_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlaySound_3.h"
int HumanPlayer_PlaySound_3(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.200000003f);
    *(context + 0x14) = sub_40aac0;
    return result;
}
