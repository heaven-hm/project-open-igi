// IGI native provenance: Render_TransformVertices_5 - 0x004D2FB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D2FB0-Render_TransformVertices_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D2FB0-Render_TransformVertices_5.hpp"
int* Render_TransformVertices_5(int* context, int* arg2, void* arg3)

{
    float data_val = *(*(arg3 + 8) + 0x80);
    int* result;

    if (!data_val)
    {
        if (context)
        {
            *context = 0;
            context[1] = 0;
            context[2] = 0;
        }

        result = arg2;

        if (result)
        {
            *result = 0;
            result[1] = 0;
            result[2] = 0;
        }
    }
    else
    {
        long double fpu0;
        result = Render_ComputeNormals_5(fpu0, context, arg2, data_val);

        if (!result)
        {
            if (context)
            {
                *context = 0;
                context[1] = 0;
                context[2] = 0;
            }

            if (arg2)
            {
                *arg2 = 0;
                arg2[1] = 0;
                arg2[2] = 0;
            }
        }
    }

    return result;
}
