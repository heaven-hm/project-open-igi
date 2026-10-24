// IGI native provenance: SplinePathDynCubeObj_OnEvent - 0x0051A280 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0051A280-SplinePathDynCubeObj_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SplinePathDynCubeObj_OnEvent.h"
int SplinePathDynCubeObj_OnEvent(int context, float arg2, int arg3, float arg4)

{
    long double fpu_val_7_1 = arg4 * arg4;
    void* data_val = *(arg2 + (arg3 << 2) + 0x68);
    void* entity_1 = *(arg2 + (arg3 << 2) + 0x6c);
    float item_20 = fpu_val_7_1 * arg4;
    float item_1c = item_20 - (fpu_val_7_1 + fpu_val_7_1) + arg4;
    arg2 = fpu_val_7_1 * 3.0f;
    long double fpu_val_6_8 = item_20;
    arg4 = fpu_val_6_8 + fpu_val_6_8 - arg2 + 1.0f;
    long double fpu_val_7_2 = item_20 - fpu_val_7_1;
    long double fpu_val_6_14 = item_20 * -2.0f + arg2;
    long double fpu_val_5_4 = fpu_val_6_14 * *(entity_1 + 0x20) + fpu_val_7_2 * *(entity_1 + 0x78)
        + arg4 * *(data_val + 0x20) + item_1c * *(data_val + 0x78);
    double item_18 = fpu_val_5_4;
    long double fpu_val_5_9 = fpu_val_6_14 * *(entity_1 + 0x28) + fpu_val_7_2 * *(entity_1 + 0x80)
        + arg4 * *(data_val + 0x28) + item_1c * *(data_val + 0x80);
    double item_10 = fpu_val_5_9;
    long double fpu_val_7_6 = fpu_val_6_14 * *(entity_1 + 0x30) + fpu_val_7_2 * *(entity_1 + 0x88)
        + arg4 * *(data_val + 0x30) + item_1c * *(data_val + 0x88);
    double item_8 = fpu_val_7_6;
    memcpy(context, &item_18, 0x18);
    return context;
}
