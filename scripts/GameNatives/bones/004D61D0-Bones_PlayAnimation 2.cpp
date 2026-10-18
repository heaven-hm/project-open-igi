// IGI native provenance: Bones_PlayAnimation - 0x004D61D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D61D0-Bones_PlayAnimation.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Bones_PlayAnimation.h"
int Bones_PlayAnimation(float context, int arg2, int arg3, int arg4, float arg5)

{
    int* entity_val = context;
    void* source_3 = entity_val[2] + arg2 * 0x84;
    void* dest_val = *(source_3 + 0x80);
    context = arg5 * 4800.0f / g_a54658;
    int* context_val = Render_DrawSplineMesh_8(*entity_val, arg2, arg3, arg4);

    if (dest_val || *(source_3 + 0x74))
    {
        long double fpu_val_7_2 = context;
        long double f0_1 = 1.0f;
        fpu_val_7_2 - f0_1;

        if (!(*(&((fpu_val_7_2 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f0_1) ? 1 : 0) << 0xa
            | (fpu_val_7_2 == f0_1 ? 1 : 0) << 0xe) + 1) & 1))
        {
            Render_DrawLightmapMesh_9(source_3, context_val, context, 0);
            int data_7 = *(source_3 + 4) | 0x20000;
            *(source_3 + 4) = data_7;
            return data_7;
        }

        if (dest_val)
            Render_DrawBoneMesh_6(dest_val);
    }

    int* data_3 = Render_DrawLightmapMesh_6();
    Render_SkinBones_6(data_3, 0, 1, context_val, 0xffffffff, 0, 0, 0, 0, 0, 0, 0);
    int entity_5 = *(source_3 + 4) & 0xfffafeff;
    *(source_3 + 0x80) = data_3;
    *(source_3 + 4) = entity_5;
    int data_5 = entity_5 | 0x20000;
    *(source_3 + 4) = data_5;
    return data_5;
}
