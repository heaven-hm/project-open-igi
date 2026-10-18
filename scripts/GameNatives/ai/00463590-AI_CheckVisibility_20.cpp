// IGI native provenance: AI_CheckVisibility_20 - 0x00463590 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463590-AI_CheckVisibility_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00463590-AI_CheckVisibility_20.hpp"
void* AI_CheckVisibility_20(void* context)

{
    *(context + 0x52c) = 1;
    return context;
}
