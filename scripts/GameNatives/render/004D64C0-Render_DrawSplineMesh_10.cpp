// IGI native provenance: Render_DrawSplineMesh_10 - 0x004D64C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D64C0-Render_DrawSplineMesh_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D64C0-Render_DrawSplineMesh_10.hpp"
int Render_DrawSplineMesh_10(void* context, int arg2)

{
    return *(Render_DrawBoneMesh_10(context, arg2) + 4);
}
