// IGI native provenance: AI_CheckVisibility_8 - 0x004560C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004560C0-AI_CheckVisibility_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_8.hpp"
int AI_CheckVisibility_8(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_4560f0;
    return result;
}
