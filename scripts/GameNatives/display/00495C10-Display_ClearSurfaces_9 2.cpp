// IGI native provenance: Display_ClearSurfaces_9 - 0x00495C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00495C10-Display_ClearSurfaces_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_9.h"
unsigned int Display_ClearSurfaces_9(HMIXEROBJ context, void* arg2, void* arg3, int arg4, int arg5)

{
    MIXERCONTROLDETAILS pmxcd;
    pmxcd.cbStruct = 0x18;
    pmxcd.cbDetails = 4;
    int entity_1 = *(arg3 + 0x10);
    pmxcd.dwControlID = *(arg3 + 4);
    int data_1 = *(arg2 + 0x1c);
    pmxcd.cChannels = data_1;
    *(&pmxcd. + 0) = entity_1;
    int item_20;
    pmxcd.paDetails = &item_20;

    if (data_1 > 0)
        item_20 = arg4;

    if (data_1 > 1)
        int item_1c_1 = arg5;

    return mixerSetControlDetails(context, &pmxcd, 0x80000000);
}
