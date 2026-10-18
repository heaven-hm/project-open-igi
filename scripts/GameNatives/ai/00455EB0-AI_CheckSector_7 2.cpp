// IGI native provenance: AI_CheckSector_7 - 0x00455EB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00455EB0-AI_CheckSector_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckSector_7.h"
int AI_CheckSector_7(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_UpdatePatrol_8;
    return result;
}
