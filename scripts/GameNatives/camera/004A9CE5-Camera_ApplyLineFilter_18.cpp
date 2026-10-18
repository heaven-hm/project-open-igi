// IGI native provenance: Camera_ApplyLineFilter_18 - 0x004A9CE5 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9CE5-Camera_ApplyLineFilter_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9CE5-Camera_ApplyLineFilter_18.hpp"
long double __fastcall __fload_withFB(int context, double* arg2)

{
    if ((*(arg2 + 4) & 0x7ff00000) != 0x7ff00000)
        return *arg2;

    int item_4 = *(arg2 + 4) | 0x7fff0000;
    int entity_val = *arg2;
    int item_6 = *(arg2 + 4) << 0xb | entity_val >> 0x15;
    *(arg2 + 4);
    return (entity_val << 0xb);
}
