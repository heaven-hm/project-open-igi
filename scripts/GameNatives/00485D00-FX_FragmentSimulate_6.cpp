// IGI native provenance: FX_FragmentSimulate_6 - 0x00485D00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485D00-FX_FragmentSimulate_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00485D00-FX_FragmentSimulate_6.hpp"
int FX_FragmentSimulate_6(int context, int arg2)

{
    return **(context + arg2 * 0x102c + 0x20);
}
