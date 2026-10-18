// IGI native provenance: MovieSubtitle_RegisterFont - 0x004151E0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004151E0-MovieSubtitle_RegisterFont.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004151E0-MovieSubtitle_RegisterFont.hpp"
int MovieSubtitle_RegisterFont(int context, int arg2, float arg3)

// Sig: void MovieSubtitle_RegisterFont(...)
// Note: Registers a font used by in-game movie subtitles (16 callsites in body;
// argc unknown - parser context). Call from subtitle/font definition blocks
// before any MovieSubtitle_RegisterText that uses it.
{
    int data_val = Script_GetArg(arg3, 0);
    char* item_42c = 0x400;
    void item_400;
    char* item_430 = &item_400;
    Sys_ResolveSymbol_23(arg3, 1, item_430, item_42c);
    Display_SetGammaRamp_2(arg2, data_val, &item_400);
    int item_418 = 2;
    int item_410 = 0;
    int item_40c = 0x40000000;
    char* item_408 = &g_567c74;
    memcpy(context, &item_418, 0x18);
    return context;
}
