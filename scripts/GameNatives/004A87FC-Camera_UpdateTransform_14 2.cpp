// IGI native provenance: Camera_UpdateTransform_14 - 0x004A87FC - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A87FC-Camera_UpdateTransform_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateTransform_14.h"
int Camera_UpdateTransform_14(short context , int arg2, int arg3)

{
    int entity_val;
    int item_8 = entity_val;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    item_8 = f0;
    short fpu_control;
    short x87status_1;
    fpu_control = __fldcw_memmem16(((~arg3 & item_8) | (arg3 & arg2)));
    return item_8;
}
