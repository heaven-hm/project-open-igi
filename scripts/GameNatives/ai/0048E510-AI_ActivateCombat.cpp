// IGI native provenance: AI_ActivateCombat - 0x0048E510 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E510-AI_ActivateCombat.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_ActivateCombat.hpp"

void* AI_ActivateCombat(void* self )

{
    *(self + 0x18) = FX_SmokeEmit_20;
    return self;
}
