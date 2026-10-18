// Native: Camera_UpdateTransform; retail VA 0x004A0110; catalog signature x86 cdecl int32_t(void).
// Artifact: retail IGI.EXE, SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: REA/Ghidra static record in 004A0110-Camera_UpdateTransform.md; catalog record at assets/BinaryNinja/IGINatives_VERIFIED.json.
// Behavior/CFG: invalidates eight matrix cache slots in order; catalog reports CFG 100% structural; Ghidra reports one block.
// Status/limits: ANALYZED; candidate returns final call result, but Ghidra pseudocode does not establish return semantics.
#include "004A0110-Camera_UpdateTransform.hpp"
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
