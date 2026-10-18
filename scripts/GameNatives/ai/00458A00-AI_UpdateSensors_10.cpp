// IGI native provenance: AI_UpdateSensors_10 - 0x00458A00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00458A00-AI_UpdateSensors_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_10.hpp"
int AI_UpdateSensors_10(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_458a30;
    return result;
}
