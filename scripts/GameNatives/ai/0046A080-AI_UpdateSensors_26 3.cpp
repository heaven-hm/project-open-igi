// IGI native provenance: AI_UpdateSensors_26 - 0x0046A080 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046A080-AI_UpdateSensors_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_26.h"
void* AI_UpdateSensors_26(void* context, int arg2, int arg3)

{
    *(context + 0x7c) = arg3;
    *(context + 0x78) = arg2;
    return context;
}
