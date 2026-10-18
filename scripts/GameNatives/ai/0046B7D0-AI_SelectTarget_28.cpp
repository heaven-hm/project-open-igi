// IGI native provenance: AI_SelectTarget_28 - 0x0046B7D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046B7D0-AI_SelectTarget_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_SelectTarget_28.hpp"
void* AI_SelectTarget_28(void* context, float arg2, int arg3)

{
    *(context + 0x78) = arg2;
    *(context + 0x7c) = arg3;
    *(context + 0x5c) = arg2;
    *(context + 0x60) = arg3;
    return context;
}
