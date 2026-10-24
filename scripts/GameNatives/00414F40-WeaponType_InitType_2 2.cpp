// IGI native provenance: WeaponType_InitType_2 - 0x00414F40 - CFG score 95.8% (PASS; structural only); runtime behavior not verified.
// Evidence: 00414F40-WeaponType_InitType_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "WeaponType_InitType_2.h"
int WeaponType_InitType_2(int context, void* arg2, int arg3, char* arg4)

{
    int data_val = DynObj_LookupHashItem_5();
    short item_10 = data_val;
    data_val = *(arg2 + 0x1c);
    char data_1;
    int entity_val;
    data_1 = TaskType_IsDerivedFrom(data_val, item_10);

    if (!data_1)
    {
        short data_2 = Render_DrawRigidMesh_12();
        entity_val = *(arg2 + 0x1c);

        if (TaskType_IsDerivedFrom(entity_val, data_2))
        {
            int* data_4 = DynObj_Missing(arg4);

            if (data_4 && Render_DrawLightmapMesh_2(*data_4) > arg3)
            {
                char* item_10_4 = arg4;
                int item_14_2 = arg3;
                void* item_18 = arg2;
                return Render_TransformVertices(context);
            }
        }
    }

    memset(context, 0, 0x18);
    return context;
}
