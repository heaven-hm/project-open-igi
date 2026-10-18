// IGI native provenance: WeaponType_RegisterType_2 - 0x00414ED0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00414ED0-WeaponType_RegisterType_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "WeaponType_RegisterType_2.h"
void** WeaponType_RegisterType_2(void** context, void* arg2, int arg3)

{
    int data_val = DynObj_LookupHashItem_5();
    short item_8 = data_val;
    data_val = *(arg2 + 0x1c);

    if (TaskType_IsDerivedFrom(data_val, item_8))
    {
        int* data_2 = *(arg2 + 0x6c);

        if (data_2 && Render_DrawLightmapMesh_2(*data_2) > arg3)
            return Render_SetupLighting(context, arg2, arg3);
    }

    *context = nullptr;
    context[1] = 0;
    context[2] = 0;
    return context;
}
