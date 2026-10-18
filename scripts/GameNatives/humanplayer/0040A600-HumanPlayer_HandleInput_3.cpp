// IGI native provenance: HumanPlayer_HandleInput_3 - 0x0040A600 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040A600-HumanPlayer_HandleInput_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_HandleInput_3.hpp"
int HumanPlayer_HandleInput_3(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);

    if (*(context + 0xc) == 0x1a)
        Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    int result = HumanPlayer_ApplyRecoil_9(source_val, 2);
    *(context + 0x14) = sub_40a670;
    return result;
}
