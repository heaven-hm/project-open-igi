// IGI native provenance: Render_BindTexture_6 - 0x004D3730 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D3730-Render_BindTexture_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D3730-Render_BindTexture_6.hpp"
int* Render_BindTexture_6(void* context)

{
    int* result = *(context + 0x80);

    if (result)
    {
        result = Render_ClipMesh_6(result);
        *(context + 0x80) = result;
    }

    return result;
}
