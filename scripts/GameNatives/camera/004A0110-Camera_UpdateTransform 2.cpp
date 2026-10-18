// IGI native provenance: Camera_UpdateTransform - 0x004A0110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A0110-Camera_UpdateTransform.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateTransform.h"
int Camera_UpdateTransform()

{
    Matrix_InvalidateSlot(g_bcad7c, g_7205b8);
    Matrix_InvalidateSlot(g_bcadac, g_723508);
    Matrix_InvalidateSlot(g_bcadac, g_7205bc);
    Matrix_InvalidateSlot(g_bcade8, g_723504);
    Matrix_InvalidateSlot(g_bcadac, g_7addd4);
    Matrix_InvalidateSlot(g_bcade8, g_7205b0);
    Matrix_InvalidateSlot(g_bcad7c, g_7ac09c);
    return Matrix_InvalidateSlot(g_bcadac, g_7234b8);
}
