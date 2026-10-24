// IGI native provenance: Render_SetupLighting_10 - 0x004D6580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D6580-Render_SetupLighting_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D6580-Render_SetupLighting_10.hpp"
int Render_SetupLighting_10(void* context, int arg2, float arg3)

{
    void* data_val = Render_DrawBoneMesh_10(context, arg2);
    *(data_val + 0x28) = g_a54658 * arg3;
    return Render_ComputeNormals_10(data_val);
}
