// IGI native provenance: Camera_ApplyBandFilter_14 - 0x004A88BE - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A88BE-Camera_ApplyBandFilter_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyBandFilter_14.h"
int Camera_ApplyBandFilter_14(short context , long double arg2 ) {
    int saved_frame;
    int* flagP = &saved_frame;
    short x87status;
    short f0;
    f0 = __fnstcw_memmem16(context);
    short item_a8 = f0;

    if (!g_546af0)
        double item_8a_1 = arg2;

    int entity_val;
    void* temp_val;
    __trandisp1(entity_val, temp_val, flagP, arg2);
    g_9361d8 = 1;
    return Camera_ApplyPostProcess_14(flagP, arg2);
}
