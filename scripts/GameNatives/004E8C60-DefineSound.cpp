// IGI native provenance: Define_Sound - 0x004E8C60 - CFG score 97.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8C60-Define_Sound.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Define_Sound.h"
int Define_Sound(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , int arg8, void* arg9, float arg10)

// Sig: void Define_Sound(...)
// Note: Defines a SOUND entry (file + attenuation params) inside sound
// definition blocks - OpenIGI reimplements it as
// SoundDefinitionRegistry.Define(SoundDefinitionKind.Sound). Call only from
// sound .qsc define blocks.
{
    void item_130;
    void* item_15c = &item_130;
    void* flags = &item_130;
    char item_14c = 1;
    SoundSys_PlaySoundSample_13(item_15c);
    char* item_160 = 0x20;
    void item_51;
    Sys_ResolveSymbol_23(arg10, 0, &item_51, item_160);
    void item_31;
    Sys_ResolveSymbol_23(arg10, 1, &item_31, 0x20);
    Profile_ValueSet(arg10, 2.80259693e-45.0f);
    double item_90 = context;
    Profile_ValueSet(arg10, 4.20389539e-45.0f);
    double item_88 = arg2;
    Profile_ValueSet(arg10, 5.60519386e-45.0f);
    double item_80 = arg3;
    Profile_ValueSet(arg10, 7.00649232e-45.0f);
    float item_6c = arg4;
    float item_15c_1 = 8.40779079e-45.0f;
    item_160 = arg10;
    Profile_ValueSet(item_160, item_15c_1);
    float item_60 = arg5;
    Profile_ValueSet(arg10, 9.80908925e-45.0f);
    float item_5c = arg6;
    int item_78 = Script_GetArg(arg10, 8);
    int item_74 = Script_GetArg(arg10, 9);
    int item_70 = Script_GetArg(arg10, 0xa);
    Profile_ValueSet(arg10, 1.54142831e-44.0f);
    float item_c = arg7;
    char data_3;
    int kind;
    data_3 = Script_GetArg(arg10, 0xc);
    kind = g_54d9b6;
    char item_54 = data_3;
    char item_53 = 1;
    QTask_HandleInput(arg9, kind, &flags);
    int item_148 = 0xd;
    int item_140 = 0;
    int item_13c = 0x402a0000;
    char* item_138 = &g_567c74;
    memcpy(arg8, &item_148, 0x18);
    return arg8;
}
