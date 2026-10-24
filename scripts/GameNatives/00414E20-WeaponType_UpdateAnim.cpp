// IGI native provenance: WeaponType_UpdateAnim - 0x00414E20 - CFG score 96.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 00414E20-WeaponType_UpdateAnim.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00414E20-WeaponType_UpdateAnim.hpp"
double* WeaponType_UpdateAnim(double* context, double* arg2, int arg3)

{
    int data_val = DynObj_LookupHashItem_5();
    short item_c = data_val;
    data_val = *(arg2 + 0x1c);

    if (!TaskType_IsDerivedFrom(data_val, item_c))
    {
        int data_5 = Render_DrawRigidMesh_12();
        short item_c_3 = data_5;
        data_5 = *(arg2 + 0x1c);

        if (TaskType_IsDerivedFrom(data_5, item_c_3))
        {
            int* data_7 = *(arg2 + 0x74);

            if (data_7 && Render_DrawLightmapMesh_2(*data_7) > arg3)
            {
                int item_c_5 = arg3;
                double* item_10_3 = arg2;
                return Render_TransformVertices(context);
            }
        }
    }
    else
    {
        int* data_2 = *(arg2 + 0x6c);

        if (data_2 && Render_DrawLightmapMesh_2(*data_2) > arg3)
            return Render_ClipMesh(context, arg2, arg3);
    }

    memset(context, 0, 0x18);
    return context;
}
