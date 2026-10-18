// IGI native provenance: AI_SelectTarget_22 - 0x00464770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00464770-AI_SelectTarget_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_SelectTarget_22.hpp"
void* AI_SelectTarget_22(void* context, float arg2, int arg3, int arg4, int arg5)

{
    *(context + 0x1dc) = arg3;
    *(context + 0x1d4) = arg4;
    *(context + 0x1d0) = arg2;
    *(context + 0x1e0) = arg5;
    return context;
}
