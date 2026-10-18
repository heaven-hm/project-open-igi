// IGI native provenance: Camera_UpdateViewCone_17 - 0x004A9C57 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9C57-Camera_UpdateViewCone_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9C57-Camera_UpdateViewCone_17.hpp"
long double __startOneArgErrorHandling(int context, int arg2, long double arg3 , int arg4 , int arg5 )

{
    int item_24 = context;
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
