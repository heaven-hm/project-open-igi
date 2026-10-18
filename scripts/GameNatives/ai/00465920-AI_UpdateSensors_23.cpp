// IGI native provenance: AI_UpdateSensors_23 - 0x00465920 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00465920-AI_UpdateSensors_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_23.hpp"
void* AI_UpdateSensors_23(void* context, int arg2, char arg3)

{
    *(context + 0x198) = arg3;

    if (!arg2)
    {
        memset(context + 0x180, 0, 0x18);
        return context;
    }

    memcpy(context + 0x180, arg2, 0x18);
    return context;
}
