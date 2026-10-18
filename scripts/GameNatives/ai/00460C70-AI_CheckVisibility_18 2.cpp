// IGI native provenance: AI_CheckVisibility_18 - 0x00460C70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00460C70-AI_CheckVisibility_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_18.h"
int AI_CheckVisibility_18(void* context, int arg2)

{
    *(context + 0x800) = arg2;
    return arg2;
}
