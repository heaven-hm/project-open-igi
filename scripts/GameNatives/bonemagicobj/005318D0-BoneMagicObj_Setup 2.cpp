// IGI native provenance: BoneMagicObj_Setup - 0x005318D0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 005318D0-BoneMagicObj_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "BoneMagicObj_Setup.h"
int BoneMagicObj_Setup(int* context, int* arg2)

{
    int* data_1 = DynObj_Missing(*arg2);

    if (data_1)
    {
        short* entity_1 = arg2[0x23];
        context[0x47] = entity_1;
        int item_20 = arg2[1];
        Render_DrawBoneMesh_12(context, *entity_1, data_1);
        DynObj_LinkDynamicCube_8(arg2[1], context, &arg2[2], context[0x28], 2);
        Render_SetupLighting_9(&context[0x1a], 0, *(context[0x47] + 4), 1);
        memcpy(&context[0x36], &arg2[8], 0x18);
        memcpy(&context[0x3c], &arg2[0x18], 0x28);
        memcpy(&context[0x1e], &arg2[0xe], 0x28);
        Render_DrawBoneMesh_7(&context[0x1e], &context[0x1a]);
        context[0x46] = arg2[0x22];
        context[0x48] = 0;
        context[0x49] = 0;
        context[0x4a] = 0;
        return 0;
    }

    int saved_base_1 = *arg2;
    Error_Show("VirModel "%s" not available");

    while (true)
        /* nop */
}
