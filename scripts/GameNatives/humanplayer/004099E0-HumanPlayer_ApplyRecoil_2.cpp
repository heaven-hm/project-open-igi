// IGI native provenance: HumanPlayer_ApplyRecoil_2 - 0x004099E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004099E0-HumanPlayer_ApplyRecoil_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_ApplyRecoil_2.hpp"
int HumanPlayer_ApplyRecoil_2(void* context, float arg2)

{
    float source_val = arg2;
    *(source_val + 0x33d) = 0;
    arg2 = Render_TransformVertices_10(source_val + 0x68, 0.0f);
    Human_StateAnimation(source_val, *(context + 0x10), 0.200000003f);
    int data_2 = *(context + 0xc) - 1;

    if (data_2 <= 0x13)
    {
        int entity_1 = 0;
        entity_1 = lookup_table_409a68[data_2];

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
    *(context + 0x14) = sub_409a80;
    return result;
}
