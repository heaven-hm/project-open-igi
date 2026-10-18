// IGI native provenance: AI_TakeCover_3 - 0x00452580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00452580-AI_TakeCover_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00452580-AI_TakeCover_3.hpp"
int AI_TakeCover_3(void* context, int arg2, float arg3, char arg4)

{
    *(context + 0x3ec8) = 0x3cd4629b;
    *(context + 0x3ecc) = 0x3cd4629b;
    long double fpu0;
    long double fpu1;
    return AI_CaFCgF(fpu0, fpu1, context, arg2, arg3, arg4);
}
