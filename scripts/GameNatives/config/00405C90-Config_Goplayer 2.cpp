// IGI native provenance: Config_Goplayer - 0x00405C90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405C90-Config_Goplayer.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_Goplayer.h"
int Config_Goplayer(int* context, int arg2)

{
    void* item_110 = &(&g_bc28c8)[arg2 * 0x159];
    char const* const item_114 = "GOPlayer";
    void item_100;
    Sys_CloseFileStream_4(context, &item_100, sprintf(&item_100, "%s("%s");\n"));
    int item_12c = (&g_bc28e8)[arg2 * 0x159];
    char const* const item_130 = "GOActiveMission";
    Sys_CloseFileStream_4(context, &item_100, sprintf(&item_100, "%s(%d);\n"));
    KEY_A(context, arg2);
    Config_Gogfxdisp(context, arg2);
    Config_English(context, arg2);
    return Config_Gosoundspeech(context, arg2);
}
