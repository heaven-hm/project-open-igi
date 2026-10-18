// IGI native provenance: Render_BindTexture_14 - 0x004DCF00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DCF00-Render_BindTexture_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_BindTexture_14.h"
int Render_BindTexture_14(int context, void* arg2, double* arg3)

{
    Render_SetupLighting_14(arg2);
    return Render_ComputeNormals_14(context, arg2, arg3);
}
