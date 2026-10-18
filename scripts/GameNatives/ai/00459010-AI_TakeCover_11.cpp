// IGI native provenance: AI_TakeCover_11 - 0x00459010 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00459010-AI_TakeCover_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_TakeCover_11.hpp"
int AI_TakeCover_11(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_459040;
    return result;
}
