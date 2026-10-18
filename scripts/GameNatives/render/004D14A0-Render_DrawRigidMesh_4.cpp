// IGI native provenance: Render_DrawRigidMesh_4 - 0x004D14A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D14A0-Render_DrawRigidMesh_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D14A0-Render_DrawRigidMesh_4.hpp"
void Render_DrawRigidMesh_4(int* context, int arg2)

{
    for (int* i = context; i; i = *i)
        i[1](arg2);
}
