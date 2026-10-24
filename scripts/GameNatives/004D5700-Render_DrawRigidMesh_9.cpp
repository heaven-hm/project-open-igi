// IGI native provenance: Render_DrawRigidMesh_9 - 0x004D5700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D5700-Render_DrawRigidMesh_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D5700-Render_DrawRigidMesh_9.hpp"
int* Render_DrawRigidMesh_9(int context, int arg2, int arg3)

{
    int* result = Render_ClipMesh_7();
    int* data_val = Render_DrawBoneMesh_9();
    Render_DrawSplineMesh_9(data_val, context, arg2, arg3);
    Render_DrawRigidMesh_8(result, 0, sub_4d5820, 0, sub_4d57c0, data_val);
    return result;
}
