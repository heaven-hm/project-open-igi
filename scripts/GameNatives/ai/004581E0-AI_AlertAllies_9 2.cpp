// IGI native provenance: AI_AlertAllies_9 - 0x004581E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004581E0-AI_AlertAllies_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_9.h"
void* AI_AlertAllies_9(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    *(source_val + 0x696) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.0f);
    int data_1 = *(context + 0xc);
    void* result;

    if (data_1 != 1 && data_1 != 3)
        result = data_1 - 0x11;

    if (data_1 == 1 || data_1 == 3 || data_1 == 0x11)
        result = Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    *(context + 0x14) = sub_45ad10;
    return result;
}
