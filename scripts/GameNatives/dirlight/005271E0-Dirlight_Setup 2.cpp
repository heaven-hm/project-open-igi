// IGI native provenance: Dirlight_Setup - 0x005271E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005271E0-Dirlight_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Dirlight_Setup.h"
int Dirlight_Setup(void* context)

{
    int data_val = *(context + 0x48);

    if (data_val)
        Render_ComputeNormals_13(data_val);

    int* source_val = g_ba1e14;
    int entity_val = *(*(context + 0x60) + source_val);
    int data_3 = g_ba1e08 - 1;
    g_ba1e08 = data_3;

    if (entity_val != data_3)
    {
        int temp_1 = g_ba1e04;
        int data_6 = *(temp_1 + (data_3 << 2)) * g_ba1e10 + g_ba1e00;
        int dest_2 = *(temp_1 + (entity_val << 2));
        int source_1 = *(source_val + data_6);
        *(temp_1 + (entity_val << 2)) = *(temp_1 + (source_1 << 2));
        *(g_ba1e04 + (source_1 << 2)) = dest_2;
        *(g_ba1e14 + data_6) = entity_val;
    }

    return Light_LightsourceNotFound(context);
}
