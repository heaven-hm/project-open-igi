// IGI native provenance: AnimCam_Update - 0x00529AA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00529AA0-AnimCam_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AnimCam_Update.h"
int AnimCam_Update(void* context, int* arg2)

{
    for (int i = 0; i < 0x58; )
    {
        i += 4;
        *(i + *arg2 - 4) = 0xc479c000;
    }

    int* item_c = arg2;
    void* item_14 = context;
    Light_ApplyLightmap_7(context);
    int* data_val;
    int entity_2;
    int temp_val;
    data_val = Light_UpdateGlobalLight_7(context, arg2);
    int* item_20 = arg2;
    return Light_U2F(data_val, temp_val, entity_2, context);
}
