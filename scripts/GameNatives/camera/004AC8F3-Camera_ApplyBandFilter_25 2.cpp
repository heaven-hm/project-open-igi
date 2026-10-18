// IGI native provenance: Camera_ApplyBandFilter_25 - 0x004AC8F3 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC8F3-Camera_ApplyBandFilter_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyBandFilter_25.h"
int* Camera_ApplyBandFilter_25(int* context, int arg2)

{
    int* dest_val = context;
    int i_1 = 3;
    int f0 = arg2 / 0x20;
    char f1 = arg2 % 0x20;
    arg2 = 0;
    int i;

    do
    {
        int data_6 = *dest_val;
        *dest_val = data_6 >> (f1 & 0x1f) | arg2;
        dest_val = &dest_val[1];
        i = i_1;
        i_1 -= 1;
        arg2 = (data_6 & ~(0xffffffff << (f1 & 0x1f))) << ((0x20 - f1) & 0x1f);
    } while (i != 1);
    int context_1 = 2;
    int* entity_5 = 8;
    int* result;
    int* f3_1;

    do
    {
        if (context_1 < f0)
        {
            result = context;
            *(entity_5 + result) = 0;
        }
        else
        {
            result = *(entity_5 - (f0 << 2) + context);
            *(entity_5 + context) = result;
        }

        context_1 -= 1;
        f3_1 = entity_5;
        entity_5 -= 4;
    } while (f3_1 - 4 >= 0);
    return result;
}
