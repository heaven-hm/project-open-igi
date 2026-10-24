// IGI native provenance: Render_BindTexture_9 - 0x004D6130 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D6130-Render_BindTexture_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D6130-Render_BindTexture_9.hpp"
int Render_BindTexture_9(float context, int arg2, int arg3, int arg4, int arg5, int arg6, float arg7)

{
    int* data_val = context;
    int* source_val = data_val[2] + arg2 * 0x84;
    context = arg7 * 4800.0f / g_a54658;
    int* data_3;

    if (arg5 >= 0)
        data_3 = Render_DrawSplineMesh_8(*data_val, arg2, arg5, arg6);
    else
        data_3 = Render_BindTexture_7(source_val);

    Render_ComputeNormals_9(source_val, Render_DrawSplineMesh_6(1, Render_DrawRigidMesh_9(arg2, arg3, arg4), 1, data_3, 0, 0, 0, 0, 0, 0), context,
        0);
    int result = source_val[1] | 0x20000;
    source_val[1] = result;
    return result;
}
