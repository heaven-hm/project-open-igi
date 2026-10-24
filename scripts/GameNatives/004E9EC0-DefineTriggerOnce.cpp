// IGI native provenance: Define_TriggerOnce - 0x004E9EC0 - CFG score 98.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E9EC0-Define_TriggerOnce.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Define_TriggerOnce.h"
int Define_TriggerOnce(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , int arg8, void* arg9, float arg10)

// Sig: void Define_TriggerOnce(...)
// Note: Defines a fire-once TRIGGER entry inside definition blocks - OpenIGI
// maps it to SoundDefinitionKind.TriggerOnce. Parser-context only.
{
    void item_138;
    void* item_164 = &item_138;
    void* flags = &item_138;
    char item_154 = 1;
    SoundSys_InitSoundSys_14(item_164);
    char* item_168 = 0x20;
    void item_55;
    Sys_ResolveSymbol_23(arg10, 0, &item_55, item_168);
    void item_35;
    Sys_ResolveSymbol_23(arg10, 1, &item_35, 0x20);
    Profile_ValueSet(arg10, 2.80259693e-45.0f);
    double item_98 = context;
    Profile_ValueSet(arg10, 4.20389539e-45.0f);
    double item_90 = arg2;
    Profile_ValueSet(arg10, 5.60519386e-45.0f);
    double item_88 = arg3;
    Profile_ValueSet(arg10, 7.00649232e-45.0f);
    float item_80 = arg4;
    float item_164_1 = 8.40779079e-45.0f;
    item_168 = arg10;
    Profile_ValueSet(item_168, item_164_1);
    float item_74 = arg5;
    Profile_ValueSet(arg10, 9.80908925e-45.0f);
    float item_70 = arg6;
    Profile_ValueSet(arg10, 1.12103877e-44.0f);
    float item_c = arg7;
    int item_68 = Script_GetArg(arg10, 9);
    int item_64 = Script_GetArg(arg10, 0xa);
    int item_60 = Script_GetArg(arg10, 0xb);
    int item_5c = Script_GetArg(arg10, 0xc);
    char item_58 = Script_GetArg(arg10, 0xd);
    int item_164_2 = 0xe;
    item_168 = arg10;
    char data_5;
    int kind;
    data_5 = Script_GetArg(item_168, item_164_2);
    kind = g_54da62;
    char item_57 = data_5;
    char item_56 = 1;
    QTask_HandleInput(arg9, kind, &flags);
    int item_150 = 0xf;
    int item_148 = 0;
    int item_144 = 0x402e0000;
    char* item_140 = &g_567c74;
    memcpy(arg8, &item_150, 0x18);
    return arg8;
}
