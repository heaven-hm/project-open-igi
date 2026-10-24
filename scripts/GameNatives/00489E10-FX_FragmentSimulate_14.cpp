// IGI native provenance: FX_FragmentSimulate_14 - 0x00489E10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489E10-FX_FragmentSimulate_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00489E10-FX_FragmentSimulate_14.hpp"
int FX_FragmentSimulate_14(void* context, int arg2)

{
    return (arg2 << 4) + *(context + 8);
}
