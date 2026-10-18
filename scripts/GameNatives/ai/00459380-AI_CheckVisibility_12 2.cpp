// IGI native provenance: AI_CheckVisibility_12 - 0x00459380 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00459380-AI_CheckVisibility_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_12.h"
void* AI_CheckVisibility_12(void* context, void* arg2)

{
    *(arg2 + 0x696) = 1;
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0xd14) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    void* result = FX_TrailUpdate_15(arg2);
    *(context + 0x14) = AI_UpdatePatrol_16;
    return result;
}
