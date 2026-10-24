// IGI native provenance: Render_FailedToCreateBoneob - 0x004D2D60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D2D60-Render_FailedToCreateBoneob.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D2D60-Render_FailedToCreateBoneob.hpp"
void* Render_FailedToCreateBoneob(int* context, int arg2)

{
    *context = arg2;
    context[1] = 0;
    void* i_1 = nullptr;
    int item_8 = 0;
    int entity_1 = *(arg2 * 0xc + &g_a542f0);
    int item_84 = 0xffffffff;

    if (entity_1)
    {
        int flagP = QHash_ValueGet();
        int data_2 = *(arg2 * 0xc + &g_a542f0);

        if (!flagP)
            QHash_ValueSet();

        context[2] = Mem_Alloc(data_2 * 0x84, 4);

        if (!flagP)
            Sys_DispatchOpcode_2();

        g_a5467c = context[2];
        g_a54638 = arg2 * 0xc + &g_a542f0;
        Render_DrawLightmapMesh_5(&item_84, 0xffffffff);
        void* i;

        for (i = i_1; i; i = *(i + 0x7c))
            *(i + 0x74) = 0;

        return i;
    }

    int saved_frame = arg2;
    context[2] = 0;
    Error_Show("Failed to create boneobj of type %d, no hierarchy loaded");

    while (true)
        /* nop */
}
