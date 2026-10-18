// IGI native provenance: Render_TransformVertices_13 - 0x004DAA60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DAA60-Render_TransformVertices_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DAA60-Render_TransformVertices_13.hpp"
int* Render_TransformVertices_13(int* context, int* arg2, int* arg3, char arg4, char arg5)

{
    int entity_9 = g_b041e8;
    int* result = *(g_b041e4 + (entity_9 << 2)) * g_b041f0 + g_b041e0;
    *(g_b041f4 + result) = entity_9;
    g_b041e8 += 1;
    *result = *context;
    result[1] = context[1];
    result[2] = context[2];
    result[3] = *arg2;
    result[4] = arg2[1];
    result[5] = arg2[2];
    result[6] = *arg3;
    result[7] = arg3[1];
    result[8] = arg3[2];
    result[9] = arg4;
    *(result + 0x25) = arg5;
    return result;
}
