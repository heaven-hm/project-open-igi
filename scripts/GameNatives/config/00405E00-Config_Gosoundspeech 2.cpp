// IGI native provenance: Config_Gosoundspeech - 0x00405E00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405E00-Config_Gosoundspeech.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_Gosoundspeech.h"
int Config_Gosoundspeech(int* context, int arg2)

{
    int source_1 = arg2 * 0x564;
    double item_410 = (&g_bc25b4)[arg2 * 0x159];
    int item_414 = (&g_bc25ba)[source_1];
    char const* const item_418 = "GOSoundSpeech";
    void item_400;
    int data_5 = Game_DataSymbolLoad(&item_400, "%s(%d, %f);\n");
    double item_410_1 = (&g_bc25b0)[arg2 * 0x159];
    int item_414_1 = (&g_bc25b9)[source_1];
    item_418 = "GOSoundMusic";
    int data_7 = Game_DataSymbolLoad(&item_418 + data_5 + 0x18, "%s(%d, %f);\n");
    double item_410_2 = (&g_bc25ac)[arg2 * 0x159];
    int dest_1 = data_5 + data_7;
    int item_414_2 = (&g_bc25b8)[source_1];
    item_418 = "GOSoundFX";
    return Sys_CloseFileStream_4(context, &item_400,
        dest_1 + Game_DataSymbolLoad(&item_418 + dest_1 + 0x18, "%s(%d, %f);\n"));
}
