// IGI native provenance: AI_UpdateSensors_12 - 0x0045A9E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045A9E0-AI_UpdateSensors_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045A9E0-AI_UpdateSensors_12.hpp"
void* AI_UpdateSensors_12(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    *(source_val + 0x696) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.0f);
    void* result = *(context + 0xc);

    if (result >= 2 && (result <= 3 || result == 0x11))
        result = Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    *(context + 0x14) = sub_457d30;
    return result;
}
