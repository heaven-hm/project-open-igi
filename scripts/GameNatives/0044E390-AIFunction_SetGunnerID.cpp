// IGI native provenance: AIFunction_SetGunnerID - 0x0044E390 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044E390-AIFunction_SetGunnerID.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044E390-AIFunction_SetGunnerID.hpp"
int AIFunction_SetGunnerID(int context, float arg2 )

// Sig: void AIFunction_SetGunnerID(int gunnerId)
// Note: Assigns which mounted-gunner slot this AI mans. Param: gunnerId =
// gunner position id. Use in AI init for turret/mounted-gun NPCs.
{
    int data_1 = Script_GetArg(arg2, 0);
    void* entity_val = g_5bda58;
    int item_18 = 1;
    *(entity_val + 0x3ac8) = data_1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
