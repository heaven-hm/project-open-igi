// IGI native provenance: AI_FireWeapon_6 - 0x004544C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004544C0-AI_FireWeapon_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_6.hpp"
int AI_FireWeapon_6(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(arg2 + 0x664) = *(arg2 + 0x688);
    int result = *(arg2 + 0x68c);
    *(arg2 + 0x668) = result;
    *(arg2 + 0x66c) = *(arg2 + 0x690);
    *(context + 0x14) = AI_MoveToNode_6;
    return result;
}
