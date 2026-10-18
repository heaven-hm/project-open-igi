// IGI native provenance: Physics_InterpolateWaypoint_2 - 0x005031F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005031F0-Physics_InterpolateWaypoint_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_InterpolateWaypoint_2.hpp"
int Physics_InterpolateWaypoint_2(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , long double arg8 , float* arg9, int* arg10, float arg11)

{
    int* source_val = arg10;
    float data_val = *source_val;
    *source_val = data_val + 1;
    Profile_ValueSet(arg11, data_val);
    *arg9 = context;
    float data_1 = *source_val;
    *source_val = data_1 + 1;
    Profile_ValueSet(arg11, data_1);
    arg9[1] = arg2;
    float data_2 = *source_val;
    *source_val = data_2 + 1;
    Profile_ValueSet(arg11, data_2);
    arg9[2] = arg3;
    float data_3 = *source_val;
    *source_val = data_3 + 1;
    Profile_ValueSet(arg11, data_3);
    arg9[3] = arg4;
    float data_4 = *source_val;
    *source_val = data_4 + 1;
    Profile_ValueSet(arg11, data_4);
    arg9[4] = arg5;
    float data_5 = *source_val;
    *source_val = data_5 + 1;
    Profile_ValueSet(arg11, data_5);
    arg9[5] = arg6;
    float data_6 = *source_val;
    *source_val = data_6 + 1;
    Profile_ValueSet(arg11, data_6);
    arg9[6] = arg7;
    float data_7 = *source_val;
    *source_val = data_7 + 1;
    Profile_ValueSet(arg11, data_7);
    arg9[7] = arg8;
    float data_8 = *source_val;
    *source_val = data_8 + 1;
    Profile_ValueSet(arg11, data_8);
    arg9[8] = context;
    float data_9 = *source_val;
    *source_val = data_9 + 1;
    Profile_ValueSet(arg11, data_9);
    arg9[9] = arg2;
    short idx = 0xa;
    int data_10 = *source_val;
    *source_val = data_10 + 1;
    int data_11 = Script_GetArg(arg11, data_10);
    int i = 0;
    arg9[0xa] = data_11;

    if (data_11 > 0)
    {
        arg10 = &arg9[0xb];

        do
        {
            Profile_ValueSet(arg11, *source_val + 6);
🚫🚫🚫            float item_14_3 = /* float item_14_3 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val + 5);
🚫🚫🚫            float item_18_4 = /* float item_18_4 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val + 4);
🚫🚫🚫            float item_1c_4 = /* float item_1c_4 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val + 3);
🚫🚫            /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫            float item_20_4 = /* float item_20_4 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val + 2);
🚫🚫            /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫            float item_24_4 = /* float item_24_4 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val + 1);
🚫🚫            /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫            float item_28_4 = /* float item_28_4 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            Profile_ValueSet(arg11, *source_val);
🚫🚫🚫            float item_2c_3 = /* float item_2c_3 = unimplemented  {fstp dword [stack_ptr], fp0} */;
            /* unimplemented  {fstp dword [stack_ptr], fp0} */
            idx += 7;
            SoundSys_SetFrequency_16(arg10, item_2c_3, item_28_4, item_24_4, item_20_4, item_1c_4, item_18_4, item_14_3);
            *source_val += 7;
            i += 1;
            arg10 = &arg10[7];
        } while (i < arg9[0xa]);
    }

    float data_20 = *source_val;
    *source_val = data_20 + 1;
    Profile_ValueSet(arg11, data_20);
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫    arg9[0x43] = /* arg9[0x43] = unimplemented  {fstp dword [context_val+0x10c], fp0} */;
    /* unimplemented  {fstp dword [context_val+0x10c], fp0} */
    float data_21 = *source_val;
    *source_val = data_21 + 1;
    Profile_ValueSet(arg11, data_21);
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_533504]} */
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_53367c]} */
🚫🚫🚫    arg9[0x44] = /* arg9[0x44] = unimplemented  {fstp dword [context_val+0x110], fp0} */;
    /* unimplemented  {fstp dword [context_val+0x110], fp0} */
    float data_22 = *source_val;
    *source_val = data_22 + 1;
    int result = Profile_ValueSet(arg11, data_22);
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_533504]} */
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_53367c]} */
🚫🚫🚫    arg9[0x45] = /* arg9[0x45] = unimplemented  {fstp dword [context_val+0x114], fp0} */;
    /* unimplemented  {fstp dword [context_val+0x114], fp0} */
    return result;
}
