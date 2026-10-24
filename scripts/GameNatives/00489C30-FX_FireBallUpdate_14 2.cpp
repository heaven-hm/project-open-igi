// IGI native provenance: FX_FireBallUpdate_14 - 0x00489C30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489C30-FX_FireBallUpdate_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FireBallUpdate_14.h"
int* FX_FireBallUpdate_14(void* context, int arg2)

{
    *(context + 0x258) = 0x3f800000;
    *(context + 0x25c) = arg2;
    int* result = FX_SmokeEmit_14(context);
    *(context + 0x2e8) = 0xffffffff;
    return result;
}
