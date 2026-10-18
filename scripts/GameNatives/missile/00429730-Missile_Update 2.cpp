// IGI native provenance: Missile_Update - 0x00429730 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00429730-Missile_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Missile_Update.h"
char* Missile_Update(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , int arg8, int* arg9)

{
    void* dest_val = *arg9;
    float context_val = arg9[1];
    (*(((SoundSys_Update3DPosition_18() * 0x180 + Script_ExecuteLevelScript_16()) << 2) + &g_a96ae0))(arg8, arg9);
    int data_5 = arg9[2];
    arg9[2] = data_5 + 1;
    *(dest_val + 0x118) = Script_GetArg(context_val, data_5);
    int data_7 = arg9[2];
    arg9[2] = data_7 + 1;
    *(dest_val + 0x11c) = Script_GetArg(context_val, data_7);
    float data_9 = arg9[2];
    arg9[2] = data_9 + 1;
    Profile_ValueSet(context_val, data_9);
    *(dest_val + 0x120) = context;
    float data_10 = arg9[2];
    arg9[2] = data_10 + 1;
    Profile_ValueSet(context_val, data_10);
    *(dest_val + 0x124) = arg2 * 0.0333333351f;
    float data_11 = arg9[2];
    arg9[2] = data_11 + 1;
    Profile_ValueSet(context_val, data_11);
    *(dest_val + 0x128) = arg3;
    float data_12 = arg9[2];
    arg9[2] = data_12 + 1;
    Profile_ValueSet(context_val, data_12);
    *(dest_val + 0x12c) = arg4 * 4096.0f;
    float data_13 = arg9[2];
    arg9[2] = data_13 + 1;
    Profile_ValueSet(context_val, data_13);
    *(dest_val + 0x130) = arg5 * 4096.0f;
    float data_14 = arg9[2];
    arg9[2] = data_14 + 1;
    Profile_ValueSet(context_val, data_14);
    *(dest_val + 0x134) = arg6;
    float data_15 = arg9[2];
    arg9[2] = data_15 + 1;
    Profile_ValueSet(context_val, data_15);
    *(dest_val + 0x138) = arg7;
    int data_16 = arg9[2];
    arg9[2] = data_16 + 1;
    Sys_ResolveSymbol_23(context_val, data_16, dest_val + 0x13c, 0x100);
    int data_17 = arg9[2];
    arg9[2] = data_17 + 1;
    Sys_ResolveSymbol_23(context_val, data_17, dest_val + 0x23c, 0x10);
    char* result = Level_LoadObjects_6(dest_val + 0x13c);
    *(dest_val + 0x24c) = result;
    return result;
}
