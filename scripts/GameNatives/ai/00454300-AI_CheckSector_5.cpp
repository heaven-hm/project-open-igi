// IGI native provenance: AI_CheckSector_5 - 0x00454300 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00454300-AI_CheckSector_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckSector_5.hpp"
int AI_CheckSector_5(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_454330;
    return result;
}
