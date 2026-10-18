// IGI native provenance: Display_SyncVSync_10 - 0x00496250 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496250-Display_SyncVSync_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SyncVSync_10.h"
int Display_SyncVSync_10(int context, int arg2, int arg3, void** arg4)

{
    Camera_SetFieldOfView_33(context + arg2 * 0xc, arg4);
    arg4[2] = arg2;
    arg4[3] = arg3;
    return arg3;
}
