// IGI native provenance: Render_ComputeNormals_14 - 0x004DCCB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DCCB0-Render_ComputeNormals_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_ComputeNormals_14.h"
int Render_ComputeNormals_14(int context, void* arg2, double* arg3)

{
    double* item_30 = arg3;
    void* item_28 = nullptr;
    int item_24 = 0;
    int item_2c = 1;
    DynObj_CreateMagicObject_11(context, Render_DrawBoneMesh_4(), &item_30, arg3, 0xffffffff, 5);

    if (item_28 && item_28 != item_24 && *(item_28 + 0x138) < *(item_28 + 0x13c))
    {
        int entity_2 = g_a5e320;

        if (entity_2 < g_a5e324)
        {
            int* data_7 = *(g_a5e31c + (entity_2 << 2)) * g_a5e328 + g_a5e318;
            *(g_a5e32c + data_7) = entity_2;
            g_a5e320 += 1;
            data_7[1] = item_28;
            data_7[2] = item_24;
            int source_2 = *(item_28 + 0x138);
            int* entity_9 =
                *(*(item_28 + 0x134) + (source_2 << 2)) * *(item_28 + 0x140) + *(item_28 + 0x130);
            *(*(item_28 + 0x144) + entity_9) = source_2;
            void* temp_2 = data_7[1];
            *(temp_2 + 0x138) += 1;
            *data_7 = entity_9;
            *entity_9 = arg2;
            *(arg2 + 0x60) = 1;
            return 1;
        }
    }

    *(arg2 + 0x60) = 0;
    return 0;
}
