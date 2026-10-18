// IGI native provenance: Render_TransformVertices_11 - 0x004D7040 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D7040-Render_TransformVertices_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D7040-Render_TransformVertices_11.hpp"
int Render_TransformVertices_11(int context, int arg2)

{
    return **(*(context * 0x28 + &g_a538f4) + (arg2 << 2));
}
