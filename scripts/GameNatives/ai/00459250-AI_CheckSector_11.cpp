// IGI native provenance: AI_CheckSector_11 - 0x00459250 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00459250-AI_CheckSector_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00459250-AI_CheckSector_11.hpp"
int AI_CheckSector_11(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    *(arg2 + 0x696) = 0;
    Human_StateAnimation(arg2, *(context + 0x10), 0.0f);
    *(arg2 + 0x664) = *(arg2 + 0x688);
    int result = *(arg2 + 0x68c);
    *(arg2 + 0x668) = result;
    *(arg2 + 0x66c) = *(arg2 + 0x690);
    *(context + 0x14) = AI_SelectTarget_15;
    return result;
}
