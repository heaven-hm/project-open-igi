// IGI native provenance: AI_UpdateOrders_3 - 0x004525B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004525B0-AI_UpdateOrders_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateOrders_3.h"
int AI_UpdateOrders_3(void* context, int arg2, float arg3, char arg4)

{
    *(context + 0x3ec8) = 0x40490fdb;
    *(context + 0x3ecc) = 0x40490fdb;
    long double fpu0;
    long double fpu1;
    return AI_CaFCgF(fpu0, fpu1, context, arg2, arg3, arg4);
}
