// IGI native provenance: Render_BlendbranchToIllegal - 0x004D33D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D33D0-Render_BlendbranchToIllegal.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_BlendbranchToIllegal.h"
int Render_BlendbranchToIllegal(int context, int arg2, void* arg3, int* arg4)

{
    int entity_val = *arg4;

    if (entity_val == 1)
    {
        void* data_val = arg4[1];
        int entity_1 = *(data_val + 8);

        if (!entity_1)
            return arg2;

        return entity_1(context, arg2, arg3, *(data_val + 0x10));
    }

    if (!entity_val)
        return Render_DrawRigidMesh_6(context, arg2, arg3, arg4[1]);

    Error_Show("BlendBranch to illegal target");

    while (true)
        /* nop */
}
