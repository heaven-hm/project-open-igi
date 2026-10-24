// IGI native provenance: Camera_ApplyLineFilter_14 - 0x004A8880 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8880-Camera_ApplyLineFilter_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_14.h"
int Camera_ApplyLineFilter_14(short context , long double arg2 , long double arg3 ) {
    int saved_frame;
    int* flagP = &saved_frame;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    short item_a8 = f0;

    if (!g_546af0)
    {
        double item_8a_1 = arg3;
        double item_82_1 = arg2;
    }

    int entity_val;
    void* temp_val;
    __trandisp2(entity_val, temp_val, flagP, arg2, arg3);
    g_9361d8 = 1;
    return Camera_ApplyPostProcess_14(flagP, arg2);
}
