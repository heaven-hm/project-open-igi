// IGI native provenance: Vehicle_InitHeli_3 - 0x00433030 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00433030-Vehicle_InitHeli_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Vehicle_InitHeli_3.h"
int Vehicle_InitHeli_3(float* context, int* arg2, int* arg3)

{
    float context_val = arg3;
    long double fpu0;
    long double fpu1;
    long double fpu2;
    long double fpu3;
    long double fpu4;
    long double fpu5;
    long double fpu6;
    long double fpu7;
    Physics_InterpolateWaypoint_2(fpu0, fpu1, fpu2, fpu3, fpu4, fpu5, fpu6, fpu7, context, arg2, context_val);
    int data_val = *arg2;
    *arg2 = data_val + 1;
    int data_1 = Script_GetArg(context_val, data_val);
    int i = 0;
    context[0x46] = data_1;

    if (data_1 > 0)
    {
        arg3 = &context[0x47];

        do
        {
            int data_2 = *arg2;
            *arg2 = data_2 + 1;
            i += 1;
            *arg3 = Script_GetArg(context_val, data_2);
            arg3 = &arg3[1];
        } while (i < context[0x46]);
    }

    int data_5 = *arg2;
    *arg2 = data_5 + 1;
    int result = Script_GetArg(context_val, data_5);
    int flagP = 0;
    context[0x54] = result;

    if (result > 0)
    {
        arg3 = &context[0x4a];

        do
        {
            int data_7 = *arg2;
            *arg2 = data_7 + 1;
            flagP += 1;
            *arg3 = Script_GetArg(context_val, data_7);
            result = context[0x54];
            arg3 = &arg3[1];
        } while (flagP < result);
    }

    return result;
}
