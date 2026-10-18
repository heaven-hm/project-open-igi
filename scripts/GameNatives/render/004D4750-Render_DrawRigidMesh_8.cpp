// IGI native provenance: Render_DrawRigidMesh_8 - 0x004D4750 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D4750-Render_DrawRigidMesh_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D4750-Render_DrawRigidMesh_8.hpp"
int* Render_DrawRigidMesh_8(int* context, int arg2, int arg3, int arg4, int arg5, int arg6)

{
    *context = arg2;
    context[1] = arg3;
    context[2] = arg4;
    context[3] = arg5;
    context[4] = arg6;
    return context;
}
