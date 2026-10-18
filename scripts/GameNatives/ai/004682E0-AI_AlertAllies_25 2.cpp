// IGI native provenance: AI_AlertAllies_25 - 0x004682E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004682E0-AI_AlertAllies_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_25.h"
int AI_AlertAllies_25(void* context, int arg2)

{
    *(context + 0x20) = arg2;
    return arg2;
}
