// IGI native provenance: Camera_ApplyPostProcess_17 - 0x004A9C40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9C40-Camera_ApplyPostProcess_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_17.h"
long double Camera_ApplyPostProcess_17(int context, int arg2, long double arg3 , int arg4 , int arg5 , int arg6 , int arg7 )

{
    int item_24 = context;
    int item_14 = arg6;
    int item_10 = arg7;
    double item_c = arg3;
    int entity_val;
    int item_20 = entity_val;
    int item_1c = arg4;
    int item_18 = arg5;
    short arg_4;
    Camera_ApplyWideScreen_27(arg2, &item_24, &arg_4);
    short x87control_1;
    short x87status_1;

    if (arg_4 != 0x27f)
        x87control_1 = __fldcw_memmem16(arg_4);
    return item_c;
}
