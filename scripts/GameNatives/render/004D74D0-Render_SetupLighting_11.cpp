// IGI native provenance: Render_SetupLighting_11 - 0x004D74D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D74D0-Render_SetupLighting_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D74D0-Render_SetupLighting_11.hpp"
long double Render_SetupLighting_11(int context, int arg2)

{
    return **(*(context * 0x28 + &g_a538f4) + (arg2 << 2)) * 0.000208333338f;
}
