// IGI native provenance: HumanPlayer_UpdateHealth_3 - 0x0040A410 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040A410-HumanPlayer_UpdateHealth_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateHealth_3.hpp"
int HumanPlayer_UpdateHealth_3(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);
    int data_1 = *(context + 0xc);

    if (data_1 == 6 || data_1 == 0xc || data_1 == 0x19)
        Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    int result = HumanPlayer_ApplyRecoil_9(source_val, 2);
    *(context + 0x14) = sub_40a490;
    return result;
}
