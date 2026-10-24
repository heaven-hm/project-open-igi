// IGI native provenance: Camera_ApplyPostProcess_15 - 0x004A8C0D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8C0D-Camera_ApplyPostProcess_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A8C0D-Camera_ApplyPostProcess_15.hpp"
long double Camera_ApplyPostProcess_15(void* context , long double arg2 , long double arg3 )

{
    *(context - 0x9e) = arg3;
    long double fpu_val_0_1 = *(context - 0x9e);

    if (!(*(context - 0x97) & 0x40))
        *(context - 0x90) = 1;
    else
        *(context - 0x90) = 7;

    return arg2 + fpu_val_0_1;
}
