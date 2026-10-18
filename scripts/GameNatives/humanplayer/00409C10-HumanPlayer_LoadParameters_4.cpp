// IGI native provenance: HumanPlayer_LoadParameters_4 - 0x00409C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00409C10-HumanPlayer_LoadParameters_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_LoadParameters_4.hpp"
int HumanPlayer_LoadParameters_4(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);
    int data_1 = *(context + 0xc);

    if (data_1 == 2 || data_1 == 9 || data_1 == 0x15)
        Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);

    int result = HumanPlayer_ApplyRecoil_9(source_val, 2);
    *(context + 0x14) = sub_409c90;
    return result;
}
