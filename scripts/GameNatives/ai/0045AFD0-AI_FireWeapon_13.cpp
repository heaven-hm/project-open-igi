// IGI native provenance: AI_FireWeapon_13 - 0x0045AFD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045AFD0-AI_FireWeapon_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_13.hpp"
int AI_FireWeapon_13(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 1;
    *(arg2 + 0x696) = 0;
    int result = Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(context + 0x14) = sub_45b900;
    return result;
}
