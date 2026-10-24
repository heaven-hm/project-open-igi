// IGI native provenance: Camera_ApplyHandiCam_14 - 0x004A88F4 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A88F4-Camera_ApplyHandiCam_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyHandiCam_14.h"
long double Camera_ApplyHandiCam_14(short context , int arg2, int arg3, int arg4, int arg5)

{
    long double fp0 = __fload(arg2, arg3);
    long double result = __fload(arg4, arg5);
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    short item_a8 = f0;
    int saved_frame;
    int entity_val;
    void* temp_val;
    __trandisp2(entity_val, temp_val, &saved_frame, result, fp0);
    Camera_ApplyWideScreen_14();
    return result;
}
