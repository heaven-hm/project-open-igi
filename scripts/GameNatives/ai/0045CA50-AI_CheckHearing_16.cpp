// IGI native provenance: AI_CheckHearing_16 - 0x0045CA50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045CA50-AI_CheckHearing_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_16.hpp"
int AI_CheckHearing_16(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45ca80;
    return result;
}
