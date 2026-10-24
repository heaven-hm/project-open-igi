// IGI native provenance: Camera_SetFieldOfView_24 - 0x004AC620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC620-Camera_SetFieldOfView_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_SetFieldOfView_24.h"
int Camera_SetFieldOfView_24(short context , int arg2, int arg3)

{
    int entity_val;
    int item_8 = entity_val;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    item_8 = f0;
    int result = (Camera_ApplyBandFilter_24(item_8) & ~arg3) | (arg2 & arg3);
    short fpu_control;
    short x87status_1;
    fpu_control = __fldcw_memmem16(Camera_ApplyWideScreen_24(result));
    return result;
}
