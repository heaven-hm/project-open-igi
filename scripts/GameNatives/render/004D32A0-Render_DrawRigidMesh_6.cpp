// IGI native provenance: Render_DrawRigidMesh_6 - 0x004D32A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D32A0-Render_DrawRigidMesh_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D32A0-Render_DrawRigidMesh_6.hpp"
int Render_DrawRigidMesh_6(int context, int arg2, void* arg3, void* arg4)

{
    void* item_14 = arg4;
    int* data_val = Render_SetupLighting_6(arg4, 0);
    int* data_1 = Render_SetupLighting_6(arg4, 1);
    int data_2 = *(arg4 + 0x14);

    if (!data_2)
    {
        int data_17;

        if (*data_val == 0xffffffff)
            data_17 = arg2;
        else
            data_17 = Render_BlendbranchToIllegal(context, arg2, arg3, data_val);

        if (*data_1 == 0xffffffff)
            return data_17;

        return Render_BlendbranchToIllegal(context, data_17, arg3, data_1);
    }

    int data_3 = data_2(*(arg4 + 0x18));

    if (data_3 != 0xffffffff)
        Render_ComputeNormals_6(arg4);

    if (!data_3)
    {
        Render_BoneobjDeleteblendno(arg4, 0);
        return Render_BlendbranchToIllegal(context, arg2, arg3, data_1);
    }

    if (data_3 == 1)
    {
        Render_BoneobjDeleteblendno(arg4, 1);
        return Render_BlendbranchToIllegal(context, arg2, arg3, data_val);
    }

    if (data_3 != 2)
    {
        *(arg3 + 4) |= 0x10000;
        return Render_BlendbranchToIllegal(context, Render_BlendbranchToIllegal(context, arg2, arg3, data_val), arg3, data_1);
    }

    Render_BoneobjDeleteblendno(arg4, 0);
    Render_BoneobjDeleteblendno(arg4, 1);
    return arg2;
}
