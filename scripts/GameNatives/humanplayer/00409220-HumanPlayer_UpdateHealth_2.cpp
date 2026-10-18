// IGI native provenance: HumanPlayer_UpdateHealth_2 - 0x00409220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00409220-HumanPlayer_UpdateHealth_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateHealth_2.hpp"
int HumanPlayer_UpdateHealth_2(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);
    int data_1 = *(context + 0xc);

    if (data_1 == 4 || data_1 == 0xd || data_1 == 0x18)
        Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    int result = HumanPlayer_ApplyRecoil_9(source_val, 3);
    *(context + 0x14) = HumanPlayer_HandleInput_2;
    return result;
}
