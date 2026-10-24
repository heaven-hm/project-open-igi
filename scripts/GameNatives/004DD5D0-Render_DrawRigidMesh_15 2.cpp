// IGI native provenance: Render_DrawRigidMesh_15 - 0x004DD5D0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004DD5D0-Render_DrawRigidMesh_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawRigidMesh_15.h"
int Render_DrawRigidMesh_15(int context, int* arg2, int* arg3)

{
    int temp_4 = g_a5e354;

    if (context == temp_4 && temp_4 == g_a538d0)
    {
        g_b12b78 = *arg2;
        g_b12b7c = arg2[1];
        g_b16c60 = *arg3;
        g_b16c64 = arg3[1];
        g_54843c = 1;
    }

    Display_CloseDevice_13(0xbcaba0, arg2, arg3);
    return (*((*g_548694 << 2) + &g_a982e0))(context, 0);
}
