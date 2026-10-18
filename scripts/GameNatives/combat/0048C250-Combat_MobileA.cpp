// IGI native provenance: Combat_MobileA - 0x0048C250 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048C250-Combat_MobileA.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048C250-Combat_MobileA.hpp"
void* Combat_MobileA(void* context, void* arg2)

{
    *(context + 0x3e50) = 0;
    *(arg2 + 0x18) = CombatKneel;
    *(arg2 + 0x1c) = Combat_MobileA;
    return arg2;
}
