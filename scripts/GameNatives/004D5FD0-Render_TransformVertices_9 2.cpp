// IGI native provenance: Render_TransformVertices_9 - 0x004D5FD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D5FD0-Render_TransformVertices_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_TransformVertices_9.h"
int* Render_TransformVertices_9(int* context, int arg2, int arg3)

{
    *context = 0;
    context[1] = arg2;
    context[2] = 0x3f800000;
    context[3] = arg3;
    return context;
}
