// IGI native provenance: Render_BindTexture_7 - 0x004D46E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D46E0-Render_BindTexture_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D46E0-Render_BindTexture_7.hpp"
int* Render_BindTexture_7(int context)

{
    int* result = Render_ClipMesh_7();
    Render_DrawRigidMesh_8(result, 0, sub_4d4780, 0, 0, context);
    return result;
}
