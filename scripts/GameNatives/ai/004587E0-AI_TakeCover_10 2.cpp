// IGI native provenance: AI_TakeCover_10 - 0x004587E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004587E0-AI_TakeCover_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_TakeCover_10.h"
int AI_TakeCover_10(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_458810;
    return result;
}
