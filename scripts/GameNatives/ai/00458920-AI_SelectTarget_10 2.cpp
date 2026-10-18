// IGI native provenance: AI_SelectTarget_10 - 0x00458920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00458920-AI_SelectTarget_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_SelectTarget_10.h"
int AI_SelectTarget_10(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_458950;
    return result;
}
