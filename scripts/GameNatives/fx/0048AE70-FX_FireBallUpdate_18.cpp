// IGI native provenance: FX_FireBallUpdate_18 - 0x0048AE70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048AE70-FX_FireBallUpdate_18.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048AE70-FX_FireBallUpdate_18.hpp"
void* FX_FireBallUpdate_18(int context, void* arg2, int arg3, int arg4)

{
    *(arg2 + 0x24) = arg4;

    if (!arg3)
        return arg2;

    return arg3(context, arg2);
}
