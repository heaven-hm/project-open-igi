// IGI native provenance: Render_BindTexture_5 - 0x004D31C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D31C0-Render_BindTexture_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_BindTexture_5.h"
int Render_BindTexture_5(float* context, float* arg2, int* arg3)

{
    int entity_val = *arg3;

    if (entity_val == 1)
    {
        int* entity_1 = arg3[1];
        int temp_val = *entity_1;

        if (temp_val)
            return temp_val(context, arg2, entity_1[4]);
    }
    else
    {
        long double fpu0;

        if (!entity_val)
            return Render_ComputeNormals_5(fpu0, context, arg2, arg3[1]);
    }

    return 0;
}
