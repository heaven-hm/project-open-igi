// IGI native provenance: Camera_UpdateViewCone_14 - 0x004A8A65 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8A65-Camera_UpdateViewCone_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A8A65-Camera_UpdateViewCone_14.hpp"
long double Camera_UpdateViewCone_14(short context , int arg2, int arg3)

{
    long double result = __fload(arg2, arg3);
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    short item_a8 = f0;
    int saved_frame;
    int entity_val;
    void* temp_val;
    __trandisp1(entity_val, temp_val, &saved_frame, result);
    Camera_ApplyWideScreen_14();
    return result;
}
