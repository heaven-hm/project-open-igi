// IGI native provenance: HumanPlayer_CheckGround_3 - 0x00409FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00409FF0-HumanPlayer_CheckGround_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckGround_3.hpp"
int HumanPlayer_CheckGround_3(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);
    int data_2 = *(context + 0xc) - 4;

    if (data_2 <= 0x13)
    {
        int entity_1 = 0;
        entity_1 = lookup_table_40a078[data_2];

        switch (entity_1)
        {
            case 0:
            {
                Render_BindTexture_10(source_val + 0x68, 0, Render_DrawLightmapMesh_10(source_val + 0x68, 0) * arg2);
                break;
            }
        }
    }

    int result = HumanPlayer_ApplyRecoil_9(source_val, 2);
    *(context + 0x14) = sub_40a090;
    return result;
}
