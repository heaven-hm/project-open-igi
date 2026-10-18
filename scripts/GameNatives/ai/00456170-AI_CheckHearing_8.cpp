// IGI native provenance: AI_CheckHearing_8 - 0x00456170 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456170-AI_CheckHearing_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_8.hpp"
int AI_CheckHearing_8(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_4561a0;
    return result;
}
