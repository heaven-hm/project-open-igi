// IGI native provenance: Render_DrawSplineMesh_9 - 0x004D5790 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004D5790-Render_DrawSplineMesh_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D5790-Render_DrawSplineMesh_9.hpp"
int* Render_DrawSplineMesh_9(int* context, int arg2, int arg3, int arg4)

{
    *context = arg2;
    context[1] = arg3;
    memcpy(&context[2], arg4, 0x14);
    return context;
}
