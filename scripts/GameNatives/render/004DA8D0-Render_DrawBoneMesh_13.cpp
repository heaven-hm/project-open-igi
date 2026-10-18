// IGI native provenance: Render_DrawBoneMesh_13 - 0x004DA8D0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004DA8D0-Render_DrawBoneMesh_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DA8D0-Render_DrawBoneMesh_13.hpp"
int Render_DrawBoneMesh_13(int context, int* arg2, int arg3)

{
    int result = g_b04260;
    int temp_val = g_b03f90 + 1;
    g_b03f90 = temp_val;

    if (result < 0xa && temp_val <= 0x17)
    {
        void* temp_1 = result * 0x28 + &g_b03e00;
        int entity_3 = g_b03df0;
        *((entity_3 << 2) + &g_b04200) = result + 0x10000;
        g_b03df0 = entity_3 + 1;
        g_b04260 = result + 1;
        memcpy(temp_1, context, 0x18);
        *(temp_1 + 0x18) = *arg2;
        *(temp_1 + 0x1c) = arg2[1];
        *(temp_1 + 0x20) = arg2[2];
        result = arg3;
        *(temp_1 + 0x24) = result;
    }

    return result;
}
