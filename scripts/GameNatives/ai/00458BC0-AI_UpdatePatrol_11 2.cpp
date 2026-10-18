// IGI native provenance: AI_UpdatePatrol_11 - 0x00458BC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00458BC0-AI_UpdatePatrol_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdatePatrol_11.h"
int AI_UpdatePatrol_11(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_458bf0;
    return result;
}
