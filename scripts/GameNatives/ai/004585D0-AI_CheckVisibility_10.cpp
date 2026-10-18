// IGI native provenance: AI_CheckVisibility_10 - 0x004585D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004585D0-AI_CheckVisibility_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_10.hpp"
int AI_CheckVisibility_10(void* context, void* arg2)

{
    *(arg2 + 0xd18) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CheckSector_13;
    return result;
}
