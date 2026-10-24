// IGI native provenance: Input_Run - 0x00507EC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00507EC0-Physics_UpdateForces_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_UpdateForces_6.h"
short* Input_Run(int* context, int arg2)

{
    context[6] = context[7];
    int data_1 = *context;

    if (data_1)
        data_1(context, arg2);

    int temp_val = context[7];
    int i = 0;
    context[5] = ~context[6] & temp_val;
    short* result = &context[8];

    do
    {
        if (!(temp_val & 1 << (i & 0x1f)))
            *result = 0;
        else
            *result += 1;

        i += 1;
        result = &result[1];
    } while (i < 0x20);

    return result;
}
