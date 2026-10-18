// IGI native provenance: MovieSubtitle_RegisterText - 0x00415260 - CFG score 97.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00415260-MovieSubtitle_RegisterText.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00415260-MovieSubtitle_RegisterText.hpp"
int MovieSubtitle_RegisterText(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , int arg7, char* arg8, float arg9)

// Sig: void MovieSubtitle_RegisterText(...)
// Note: Registers one subtitle text entry (string + timing) shown over
// movies/briefings (argc unknown - parser context). Call after the font
// registration it should use.
{
    int item_44c = Script_GetArg(arg9, 0);
    void item_400;
    char* item_450 = Sys_ResolveSymbol_23(arg9, 1, &item_400, 0x400);
    int item_448 = Script_GetArg(arg9, 2);
    int item_444 = Script_GetArg(arg9, 3);
    Profile_ValueSet(arg9, 5.60519386e-45.0f);
    float item_438 = context;
    Profile_ValueSet(arg9, 7.00649232e-45.0f);
    float item_434 = arg2;
    Profile_ValueSet(arg9, 8.40779079e-45.0f);
    long double fpu2;
    float item_430 = fpu2;
    char item_41c = Script_GetArg(arg9, 7);
    Profile_ValueSet(arg9, 1.12103877e-44.0f);
    float item_42c = arg3;
    Profile_ValueSet(arg9, 1.26116862e-44.0f);
    float item_428 = arg4;
    int item_440 = Script_GetArg(arg9, 0xa);
    Profile_ValueSet(arg9, 1.54142831e-44.0f);
    float item_424 = arg5;
    int item_43c = Script_GetArg(arg9, 0xc);
    Profile_ValueSet(arg9, 1.821688e-44.0f);
    float item_420 = arg6;
    Display_CreateSurface_2(arg8, &item_450);
    int item_418 = 0xe;
    int item_410 = 0;
    int item_40c = 0x402c0000;
    char* item_408 = &g_567c74;
    memcpy(arg7, &item_418, 0x18);
    return arg7;
}
