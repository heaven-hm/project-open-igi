// IGI native provenance: Render_BindTexture_10 - 0x004D65B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D65B0-Render_BindTexture_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D65B0-Render_BindTexture_10.hpp"
void* Render_BindTexture_10(void* context, int arg2, int arg3)

{
    void* result = Render_DrawBoneMesh_10(context, arg2);
    *(result + 0x2c) = arg3;
    return result;
}
