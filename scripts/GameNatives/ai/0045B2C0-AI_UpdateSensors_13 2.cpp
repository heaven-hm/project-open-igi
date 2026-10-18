// IGI native provenance: AI_UpdateSensors_13 - 0x0045B2C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045B2C0-AI_UpdateSensors_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_13.h"
int AI_UpdateSensors_13(void* context, void* arg2)

{
    *(arg2 + 0xd18) = 0;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = AI_CheckSector_13;
    return result;
}
