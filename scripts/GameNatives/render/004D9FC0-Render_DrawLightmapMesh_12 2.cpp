// IGI native provenance: Render_DrawLightmapMesh_12 - 0x004D9FC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D9FC0-Render_DrawLightmapMesh_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawLightmapMesh_12.h"
struct status_msg* Render_DrawLightmapMesh_12(double* context, double* arg2, int* arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6, unsigned int arg7, unsigned int arg8)

{
    int kind;
    kind = g_a5e304;
    struct status_msg* result = QTask_HandleInput(nullptr, kind, 0);
    result->head[0x1b] = arg4;

    if (!arg3)
    {
        result->head[0x20] = 0;
        result->head[0x21] = 0;
        result->head[0x22] = 0;
    }
    else
    {
        result->head[0x20] = *arg3;
        result->head[0x21] = arg3[1];
        result->head[0x22] = arg3[2];
    }

    result->head[0x1c] = arg5;
    result->head[0x1e] = arg7;
    result->head[0x1d] = arg6;
    result->head[0x1f] = arg8;
    result->head[0x1a] = g_a44344;
    result->head[0x23] = 0;
    double item_20 = (*arg2 - *context) * 0.5 + *context;
    double item_18 = (arg2[1] - context[1]) * 0.5 + context[1];
    double item_10 = (arg2[2] - context[2]) * 0.5 + context[2];
    DynObj_RemoveSymbol_8(result->head[0x1a], result, &item_20, context, arg2, 4);
    return result;
}
