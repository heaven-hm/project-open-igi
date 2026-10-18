// IGI native provenance: AI_CheckHearing_15 - 0x0045BD90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045BD90-AI_CheckHearing_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045BD90-AI_CheckHearing_15.hpp"
int AI_CheckHearing_15(void* context, void* arg2)

{
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45bdc0;
    return result;
}
